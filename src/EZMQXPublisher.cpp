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

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string HEALTH = "/health";

static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";

static const std::string RESULT_KEY = "result";
static const std::string RESULT_SUCCESS = "success";
static const std::string RESULT_DUPLICATED = "duplicated";

static const std::string TOPIC_PATTERN = "(\/[a-zA-Z0-9-_*.]+)+";

static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

static std::function<void(ezmq::EZMQErrorCode code)> ezmqCb = [](ezmq::EZMQErrorCode code)->void{std::cout<<"errCb"<<std::endl; return;};

EZMQX::Publisher::Publisher(int optionalPort) : terminated(false), localPort(0), token(""), ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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
        EZMQX_LOG_V(ERROR, TAG, "%s Could not start publisher", __func__);
        throw EZMQX::Exception("Could not start publisher", EZMQX::UnKnownState);
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
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

    if (tmp.find(TOPIC_WILD_CARD) != std::string::npos && tmp.find(TOPIC_WILD_PATTERNN) == std::string::npos)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid topic %s", __func__, topic);
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

    try
    {
        Json::Value value;
        value[PAYLOAD_TOPIC] = regTopic.getTopic();
        value[PAYLOAD_ENDPOINT] = regTopic.getEndpoint().toString();
        value[PAYLOAD_SCHEMA] = regTopic.getSchema();

        Json::FastWriter writer;
        tmp = writer.write(value);
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not build json payload %s", __func__, regTopic.getTopic());
        throw EZMQX::Exception("Could not build json payload", EZMQX::UnKnownState);
    }

    try
    {
        EZMQX::SimpleRest rest;
        tmp = rest.Post(ctx->getTnsAddr() + PREFIX + TOPIC, tmp);
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest post request %s", __func__, ctx->getTnsAddr() + PREFIX + TOPIC, tmp);
        throw EZMQX::Exception("Could not send rest post request", EZMQX::UnKnownState);
    }

    try
    {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(tmp, root))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json response %s", __func__, tmp);
            throw EZMQX::Exception("Could not parse json response", EZMQX::UnKnownState);
        }
        else
        {
            tmp = root[RESULT_KEY].asString();
        }
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json response %s", __func__, regTopic.getTopic());
        throw EZMQX::Exception("Could not parse json response", EZMQX::UnKnownState);
    }

    if (tmp.compare(RESULT_SUCCESS) == 0)
    {
        ctx->insertTopic(regTopic.getTopic());
        return;
    }
    else if (tmp.compare(RESULT_DUPLICATED) == 0)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not register topic: duplicated topic", __func__);
        throw EZMQX::Exception("Could not register topic: duplicated topic", EZMQX::UnKnownState);
    }
    else
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not register topic: unknown state", __func__);
        throw EZMQX::Exception("Could not register topic: unknown state", EZMQX::UnKnownState);
    }

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
            ctx->releaseDynamicPort(localPort);

            if (ctx->isTnsEnabled())
            {
                ctx->deleteTopic(topic.getTopic());
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