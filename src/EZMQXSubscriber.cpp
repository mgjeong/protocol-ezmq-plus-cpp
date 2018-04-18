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

static const std::string TOPIC_PATTERN = "(\/[a-zA-Z0-9-_*.]+)+";

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";

static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";
static const std::string QUERY_PARAM = "topic=";

static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

EZMQX::Subscriber::Subscriber() : que(new EZMQX::BlockingQue()), terminated(false), token(""), ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

EZMQX::Subscriber::~Subscriber()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Subscriber::initialize(const std::string &topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    validateTopic(topic);

    std::list<EZMQX::Topic> verified;

    if (ctx->isTnsEnabled())
    {
        verifyTopics(topic, verified);

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

void EZMQX::Subscriber::initialize(const std::list<EZMQX::Topic> &topics)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // create thread and push to blocked
    mThread = new std::thread(&EZMQX::Subscriber::handler, this);

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getTopic();

        // find Aml rep
        try
        {
            std::cout << "Topic: " << topic_str << " Model_Id: " << topic.getSchema() << " Endpoint: " << topic.getEndpoint().toString() << std::endl;
            std::shared_ptr<AML::Representation> rep = ctx->getAmlRep(topic.getSchema());

            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getSchema())));
        }
        catch(...)
        {
            throw EZMQX::Exception("Could not found Aml Rep", EZMQX::UnKnownState);
        }

        EZMQX::Endpoint ep = topic.getEndpoint();

        ezmq::EZMQSubscriber* sub = new ezmq::EZMQSubscriber(ep.getAddr(), ep.getPort(), [](const ezmq::EZMQMessage &event)->void{ return;}, std::bind(&EZMQX::Subscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));

        if (!sub)
        {
            // throw exception
        }

        subscribers.push_back(sub);

        ezmq::EZMQErrorCode ret = sub->start();

        if (ezmq::EZMQ_OK != ret)
        {
            // throw exception
        }

        ret = sub->subscribe(topic_str);

        if (ezmq::EZMQ_OK != ret)
        {
            // throw exception
        }

        storedTopics.push_back(topic);
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

void EZMQX::Subscriber::verifyTopics(const std::string &topic, std::list<EZMQX::Topic> &verified)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // send rest
    std::string tmp;
    try
    {
        EZMQX::SimpleRest rest;
        tmp = rest.Get(ctx->getTnsAddr() + PREFIX + TOPIC, QUERY_PARAM + topic);
    }
    catch (...)
    {
        throw EZMQX::Exception("Could not send rest get request", EZMQX::UnKnownState);
    }

    // parse json
    try
    {
        Json::Value props;
        Json::Reader reader;

        // try parse json
        if((!reader.parse(tmp, props)) && props.type() != Json::arrayValue)
        {
            throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
        }
        else
        {
            // access array
            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
            {
                // get Topic
                if (props[i].isMember(PAYLOAD_TOPIC) && props[i].isMember(PAYLOAD_SCHEMA) && props[i].isMember(PAYLOAD_ENDPOINT))
                {
                    verified.push_back(EZMQX::Topic(props[i][PAYLOAD_TOPIC].asString(), props[i][PAYLOAD_SCHEMA].asString(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
                }
            }

        }
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
    }

    // made list


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
            if (mThread)
            {
                mThread->join();
            }
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