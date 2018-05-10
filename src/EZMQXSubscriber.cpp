#include <EZMQXSubscriber.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>
#include <EZMQXRest.h>
#include <iostream>
#include <json/reader.h>
#include <EZMQXBlockingQue.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXSubscriber"
#define SLASH '/'
#define DOUBLE_SLASH "//"

static const std::string TNS_KNOWN_PORT = "48323";
static const std::string COLLON = ":";

static const std::string TOPIC_PATTERN = "(\\/[a-zA-Z0-9-_*.]+)+";

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";

static const std::string QUERY_NAME = "name=";
static const std::string QUERY_HIERARCHICAL = "&hierarchical=";
static const std::string QUERY_TRUE = "yes";
static const std::string QUERY_FALSE = "no";

static const std::string PAYLOAD_TOPICS = "topics";
static const std::string PAYLOAD_NAME = "name";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_DATAMODEL = "datamodel";

static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

EZMQX::Subscriber::Subscriber() : terminated(false), ctx(EZMQX::Context::getInstance()), token(""), que(new EZMQX::BlockingQue()), mThread(std::thread(&EZMQX::Subscriber::handler, this))
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

EZMQX::Subscriber::~Subscriber()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Subscriber::initialize(const std::string &topic, bool isHierarchical)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    validateTopic(topic);

    std::list<EZMQX::Topic> verified;

    if (ctx->isTnsEnabled())
    {
        verifyTopics(topic, verified, isHierarchical);

        if (verified.empty())
        {
            throw EZMQX::Exception("Could not find matched topic", EZMQX::NoTopicMatched);
        }
    }
    else
    {
        throw EZMQX::Exception("TNS not available", EZMQX::TnsNotAvailable);
    }

    try
    {
        Subscriber::initialize(verified);
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }    
}

void EZMQX::Subscriber::getSession(EZMQX::Topic topic)
{
    EZMQX::Endpoint ep = topic.getEndpoint();
    ezmq::EZMQSubscriber* subCtx = nullptr;

    try
    {
        subCtx = new ezmq::EZMQSubscriber(ep.getAddr(), ep.getPort(),
            std::bind(&EZMQX::Subscriber::internalSubCb, this, "", std::placeholders::_1),
            std::bind(&EZMQX::Subscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));

        if (!subCtx)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not connect with endpoint %s ", __func__, ep.toString().c_str());
            throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::UnKnownState);
        }

        ezmq::EZMQErrorCode ret = subCtx->start();

        if (ezmq::EZMQ_OK != ret)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not start session with endpoint %s ", __func__, ep.toString().c_str());
            throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::UnKnownState);
        }

        ret = subCtx->subscribe(topic.getName());

        if (ezmq::EZMQ_OK != ret)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not subscribe with endpoint %s ", __func__, ep.toString().c_str());
            throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::UnKnownState);
        }
    }
    catch(const EZMQX::Exception &e)
    {
        if (subCtx)
        {
            delete subCtx;
        }

        throw e;
    }
    catch(...)
    {
        if (subCtx)
        {
            delete subCtx;
        }

        throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::UnKnownState);
    }

    subscribers.push_back(subCtx);
}

void EZMQX::Subscriber::initialize(const std::list<EZMQX::Topic> &topics)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getName();

        // find Aml rep
        try
        {
            std::shared_ptr<AML::Representation> rep = ctx->getAmlRep(topic.getDatamodel());
            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getDatamodel())));
        }
        catch(...)
        {
            throw EZMQX::Exception("Could not found Aml Rep", EZMQX::UnKnownState);
        }

        try
        {
            getSession(topic);
        }
        catch(const EZMQX::Exception &e)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s exception: %s", __func__, e.what());

            // clear all subscribers
            for (std::list<ezmq::EZMQSubscriber*>::iterator subItr = subscribers.begin() ; subItr != subscribers.end(); subItr++)
            {
                if (*subItr)
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s try ezmq subscribers terminated", __func__);
                    delete *subItr;
                }
            }

            throw e;
        }

        storedTopics.push_back(topic);
        EZMQX_LOG_V(DEBUG, TAG, "%s Topic: %s Model_Id: %s Endpoint: %s ", __func__, topic.getName().c_str(), topic.getDatamodel().c_str(), topic.getEndpoint().toString().c_str());
    }

    return;
}

void EZMQX::Subscriber::handler()
{
    while(1)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        std::pair<std::string, std::string> payload;

        if (!que || que->isTerminated())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Que is terminated exit handler", __func__);
            return;
        }

        // aml parsing here
        try
        {
            if (!que)
            {
                throw EZMQX::Exception("que is not initiated", EZMQX::UnKnownState);
            }

            que->deQue(payload);

            if (que->isTerminated())
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Que is terminated exit handler", __func__);
                return;
            }

            if (payload.first.empty() || payload.second.empty())
            {
                throw EZMQX::Exception("empty payload", EZMQX::UnKnownState);
            }

            auto itr = repDic.find(payload.first);
            std::cout << payload.first << std::endl;
            if (itr == repDic.end())
            {
                throw EZMQX::Exception("Could not find Aml rep", EZMQX::UnKnownState);
            }
            else
            {
                AML::AMLObject *obj = itr->second->ByteToData(payload.second);

                if (!obj)
                {
                    throw EZMQX::Exception("Could not convert byte to AMLObject", EZMQX::UnKnownState);
                }

                // call subCb
                cb(payload.first, obj);
            }
        }
        catch(const std::exception &e)
        {
            // call errCb
            EZMQX_LOG_V(DEBUG, TAG, "%s exception on subscriber callback thread: %s", __func__, e.what());
            AML::AMLObject *obj = nullptr;
            cb(payload.first, obj);
        }
    }

    return;
}

void EZMQX::Subscriber::internalSubCb(std::string topic, const ezmq::EZMQMessage &event)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    if (event.getContentType() == ezmq::EZMQ_CONTENT_TYPE_BYTEDATA && !topic.empty())
    {
        const ezmq::EZMQByteData &bytes= dynamic_cast<const ezmq::EZMQByteData&>(event);
        std::string payload((const char*)(bytes.getByteData()), bytes.getLength());
        que->inQue(std::make_pair(topic, payload));
    }
    else
    {
        //error handle
    }
    return;
}

void EZMQX::Subscriber::validateTopic(const std::string& topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    //validate topic
    std::string tmp = topic;

    // simple grammer check
    if (tmp.front() != SLASH || tmp.back() == SLASH || tmp.find(DOUBLE_SLASH) != std::string::npos)
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

    if (tmp.find(TOPIC_WILD_CARD) != std::string::npos && tmp.find(TOPIC_WILD_PATTERNN) == std::string::npos)
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

//Regex support is supported from  gcc-4.9 and higher
#if defined(EZMQX_GCC_VERSION) && EZMQX_GCC_VERSION >= 40900
    std::regex pattern(TOPIC_PATTERN);

    if (!std::regex_match(tmp, pattern))
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
#endif
}

void EZMQX::Subscriber::verifyTopics(const std::string &topic, std::list<EZMQX::Topic> &verified, bool isHierarchical)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // send rest
    std::string tmp;
    EZMQX::RestResponse restResp;
    try
    {
        std::string query = QUERY_NAME + topic + QUERY_HIERARCHICAL + (isHierarchical == true ? QUERY_TRUE : QUERY_FALSE);
        restResp = EZMQX::RestService::Get(ctx->getTnsAddr() + COLLON + TNS_KNOWN_PORT + PREFIX + TOPIC, query);

        if (restResp.getStatus() == EZMQX::Success)
        {
            tmp = restResp.getPayload();
            EZMQX_LOG_V(DEBUG, TAG, "%s topic %s query successfully", __func__, topic.c_str());
            EZMQX_LOG_V(DEBUG, TAG, "%s", restResp.getPayload().c_str());
        }
        else if (restResp.getStatus() == EZMQX::BadRequest)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not query topic successfully : BadRequest", __func__);
            throw EZMQX::Exception("Could not qeury topic: BadRequest", EZMQX::RestError);
        }
        else if (restResp.getStatus() == EZMQX::NotFound)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not qeury topic successfully : NotFound", __func__);
            throw EZMQX::Exception("Could not qeury topic: NotFound", EZMQX::RestError);
        }
        else if (restResp.getStatus() == EZMQX::InternalError)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not qeury topic successfully : Internal Server Error", __func__);
            throw EZMQX::Exception("Could not qeury topic: Internal Server Error", EZMQX::RestError);
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not qeury topic successfully : unknown rest error code %d", __func__, restResp.getStatus());
            throw EZMQX::Exception("Could not qeury topic: unknown rest error", EZMQX::RestError);
        }
    }
    catch(const EZMQX::Exception& e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest get request", __func__);
        throw EZMQX::Exception("Could not send rest get request", EZMQX::UnKnownState);
    }

    // parse json
    try
    {
        std::string errors;
        Json::Value root;
        Json::Value props;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

        // try parse json
        if (!(reader->parse(tmp.c_str(), tmp.c_str() + tmp.size(), &root, &errors)))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object error : %s", __func__, errors.c_str());
            throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s json object parsed", __func__);
            props = root[PAYLOAD_TOPICS];
            EZMQX_LOG_V(ERROR, TAG, "%s props parsed", __func__);
            // access array
            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
            {
                // get Topic
                if (props[i].isMember(PAYLOAD_NAME) && props[i].isMember(PAYLOAD_DATAMODEL) && props[i].isMember(PAYLOAD_ENDPOINT))
                {
                    verified.push_back(EZMQX::Topic(props[i][PAYLOAD_NAME].asString(), props[i][PAYLOAD_DATAMODEL].asString(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
                }
            }
        }
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(const std::exception& e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object exception: %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object", __func__);
        throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
    }

    return;
}

bool EZMQX::Subscriber::isTerminated()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // atomically
    return terminated.load();
}

void EZMQX::Subscriber::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s try terminate ezmq subscribers", __func__);
            // release resource
            for (std::list<ezmq::EZMQSubscriber*>::iterator itr = subscribers.begin() ; itr != subscribers.end(); itr++)
            {
                if (*itr)
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s try ezmq subscribers terminated", __func__);
                    delete *itr;
                }
            }

            EZMQX_LOG_V(DEBUG, TAG, "%s try stop callback thread", __func__);
            que->stop();
            mThread.join();
            EZMQX_LOG_V(DEBUG, TAG, "%s callback thread stoped", __func__);
            delete que;
            terminated.store(true);
        }
        else
        {
            EZMQX_LOG_V(INFO, TAG, "%s Subscriber already terminated", __func__);
        }
    }
    // mutex unlock
    return;
}

std::list<EZMQX::Topic> EZMQX::Subscriber::getTopics()
{
    return storedTopics;
}