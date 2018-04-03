#include <EZMQXContext.h>
#include <EZMQXPublisher.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQXRest.h>
#include <iostream>
#include <json/writer.h>
#include <json/reader.h>

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string HEALTH = "/health";

static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";

static const std::string RESULT_KEY = "result";
static const std::string RESULT_SUCCESS = "success";
static const std::string RESULT_DUPLICATED = "duplicated";

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
static std::function<void(ezmq::EZMQErrorCode code)> ezmqCb = [](ezmq::EZMQErrorCode code)->void{std::cout<<"errCb"<<std::endl; return;};

EZMQX::Publisher::Publisher(int optionalPort) : terminated(false), localPort(0), token("")
{
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
    pubCtx = std::make_shared<ezmq::EZMQPublisher>(localPort, ezmqCb, ezmqCb, ezmqCb);

    // check error and throw exception
    if (ezmq::EZMQ_OK != pubCtx->start())
    {
        throw EZMQX::Exception("Could not start publisher", EZMQX::UnKnownState);
    }
}

EZMQX::Publisher::~Publisher()
{
    terminate();
}

void EZMQX::Publisher::registerTopic(EZMQX::Topic& regTopic)
{
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
        throw EZMQX::Exception("Could not build json payload", EZMQX::UnKnownState);
    }

    try
    {
        EZMQX::SimpleRest rest;
        tmp = rest.Post(ctx->getTnsAddr() + PREFIX + TOPIC, tmp);
    }
    catch (...)
    {
        throw EZMQX::Exception("Could not send rest post request", EZMQX::UnKnownState);
    }

    try
    {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(tmp, root))
        {
            throw EZMQX::Exception("Could not parse json response", EZMQX::UnKnownState);
        }
        else
        {
            tmp = root[RESULT_KEY].asString();
        }
    }
    catch (...)
    {
        throw EZMQX::Exception("Could not parse json response", EZMQX::UnKnownState);
    }

    if (tmp.compare(RESULT_SUCCESS) == 0)
    {
        return;
    }
    else if (tmp.compare(RESULT_DUPLICATED) == 0)
    {
        throw EZMQX::Exception("Could not register topic: duplicated topic", EZMQX::UnKnownState);
    }
    else
    {
        throw EZMQX::Exception("Could not register topic: unknown state", EZMQX::UnKnownState);
    }

    return;
}

bool EZMQX::Publisher::isTerminated()
{
    // atomically
    return terminated.load();
}

void EZMQX::Publisher::terminate()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // release resource
            ctx->releaseDynamicPort(localPort);
        }
        else
        {
            throw EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    return;
}