#include <EZMQXContext.h>
#include <EZMQXPublisher.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQXRest.h>
#include <iostream>
#include <json/writer.h>
#include <json/reader.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXPublisher"
#define SLASH '/'
#define DOUBLE_SLASH "//"

static const std::string TNS_KNOWN_PORT = "48323";
static const std::string COLLON = ":";

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string HEALTH = "/health";

static const std::string PAYLOAD_OPTION = "indentation";
static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_NAME = "name";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_DATAMODEL = "datamodel";

static const std::string PAYLOAD_KEEPALIVE_INTERVAL = "ka_interval";

static const std::string TOPIC_PATTERN = "(\\/[a-zA-Z0-9-_*.]+)+";

static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

static std::function<void(ezmq::EZMQErrorCode code)> ezmqCb = [](ezmq::EZMQErrorCode code)->void{std::cout<<"errCode"<< code <<std::endl; return;};

EZMQX::Publisher::Publisher(int optionalPort) : terminated(false), ctx(EZMQX::Context::getInstance()), localPort(0), token("")
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create publisher context not initialized", EZMQX::NotInitialized);
    }

    bool isStandAlone = ctx->isStandAlone();
    //validate topic
    try
    {
        if (isStandAlone)
        {
            localPort = optionalPort;
        }
        else
        {
            localPort = ctx->assignDynamicPort();
        }
    }
    catch(...)
    {
        // throw maximum port exceed exception
    }

    // create ezmq publisher
    // ezmq error callback should provide shared pointer in callback
    pubCtx = new ezmq::EZMQPublisher(localPort, ezmqCb, ezmqCb, ezmqCb);

    // check error and throw exception
    if (!pubCtx)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not create publisher", __func__);
        throw EZMQX::Exception("Could not create publisher", EZMQX::UnKnownState);
    }

    if (ezmq::EZMQ_OK != pubCtx->start())
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not start publisher", __func__);
        throw EZMQX::Exception("Could not start publisher", EZMQX::UnKnownState);
    }
}

EZMQX::Publisher::~Publisher()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Publisher::validateTopic(const std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    //validate topic
    std::string tmp = topic;

    // simple grammer check
    if (tmp.front() != SLASH || tmp.back() == SLASH || tmp.find(DOUBLE_SLASH) != std::string::npos)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid topic %s", __func__, topic.c_str());
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

    if (tmp.find(TOPIC_WILD_CARD) != std::string::npos && tmp.find(TOPIC_WILD_PATTERNN) == std::string::npos)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid topic %s", __func__, topic.c_str());
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

//Regex support is supported from  gcc-4.9 and higher
#if defined(EZMQX_GCC_VERSION) && EZMQX_GCC_VERSION >= 40900
    std::regex pattern(TOPIC_PATTERN);

    if (!std::regex_match(tmp, pattern))
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
#endif
}

void EZMQX::Publisher::registerTopic(EZMQX::Topic& regTopic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    this->topic = regTopic;

    if (!ctx->isTnsEnabled())
    {
        return;
    }

    std::string tmp;
    int interval = 0;
    EZMQX::RestResponse resp;

    try
    {
        Json::Value root = Json::Value(Json::objectValue);
        root[PAYLOAD_TOPIC] = Json::Value(Json::objectValue);

        root[PAYLOAD_TOPIC][PAYLOAD_NAME] = regTopic.getName();
        root[PAYLOAD_TOPIC][PAYLOAD_ENDPOINT] = regTopic.getEndpoint().toString();
        root[PAYLOAD_TOPIC][PAYLOAD_DATAMODEL] = regTopic.getDatamodel();

        Json::StreamWriterBuilder builder;
        builder[PAYLOAD_OPTION] = "";

        tmp = Json::writeString(builder, root);
        EZMQX_LOG_V(DEBUG, TAG, "%s payload %s", __func__, tmp.c_str());
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not build json payload %s", __func__, regTopic.getName().c_str());
        throw EZMQX::Exception("Could not build json payload", EZMQX::UnKnownState);
    }

    try
    {
        resp = EZMQX::RestService::Post(ctx->getTnsAddr() + COLLON + TNS_KNOWN_PORT + PREFIX + TOPIC, tmp);
    }
    catch(const EZMQX::Exception &e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch (...)
    {
        throw EZMQX::Exception("Could not send rest post request", EZMQX::UnKnownState);
    }

    if (resp.getStatus() == EZMQX::Created)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s topic %s register successfully", __func__, regTopic.getName().c_str());
    }
    else if (resp.getStatus() == EZMQX::Conflict)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not register topic: duplicated", __func__);
        throw EZMQX::Exception("Could not register topic: duplicated", EZMQX::RestError);
    }
    else if (resp.getStatus() == EZMQX::InternalError)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not register topic: Internal Server Error", __func__);
        throw EZMQX::Exception("Could not register topic: Internal Server Error", EZMQX::RestError);
    }
    else
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not register topic: unknown rest error", __func__);
        throw EZMQX::Exception("Could not register topic: unknown rest error", EZMQX::RestError);
    }

    try
    {
        std::string response = resp.getPayload();
        std::string errors;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(response.c_str(), response.c_str() + response.size(), &root, &errors))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json response %s", __func__, response.c_str());
            throw EZMQX::Exception("Could not parse json response", EZMQX::RestError);
        }
        else
        {
            interval = root[PAYLOAD_KEEPALIVE_INTERVAL].asInt();

            if (interval < 1)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Invalid keepAlive option parsed", __func__);
                throw EZMQX::Exception("Invalid keepAlive option parsed", EZMQX::RestError);
            }
            else
            {
                if (ctx->getKeepAliveInterval() < 1)
                {
                    ctx->updateKeepAliveInterval(interval);
                }

                ctx->insertTopic(regTopic.getName());
            }
        }
    }
    catch (const EZMQX::Exception& e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json response %s", __func__, regTopic.getName().c_str());
        throw EZMQX::Exception("Could not parse json response", EZMQX::UnKnownState);
    }

    // TODO
    // Update keepAlive interval

    return;
}

bool EZMQX::Publisher::isTerminated()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // atomically
    return terminated.load();
}

void EZMQX::Publisher::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // release resource
            if (!(ctx->isStandAlone()))
            {
                ctx->releaseDynamicPort(localPort);
            }

            if (ctx->isTnsEnabled())
            {
                ctx->deleteTopic(topic.getName());
            }

            delete pubCtx;
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Publisher terminated", __func__);
            throw EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    return;
}

EZMQX::Topic EZMQX::Publisher::getTopic()
{
    return topic;
}