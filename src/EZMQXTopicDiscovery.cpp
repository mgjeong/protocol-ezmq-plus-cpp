#include <list>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>
#include <EZMQXContext.h>
#include <EZMQXRest.h>
#include <json/reader.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXTopicDiscovery"

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string QUERY_PARAM = "topic=";
static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";

EZMQX::TopicDiscovery::TopicDiscovery() : ctx(EZMQX::Context::getInstance()){}

EZMQX::TopicDiscovery::~TopicDiscovery(){}

void EZMQX::TopicDiscovery::validateTopic(std::string& topic, std::list<EZMQX::Topic>& topics)
{
    std::string tmp;
    // send rest
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
                    topics.push_back(EZMQX::Topic(props[i][PAYLOAD_TOPIC].asString(), props[i][PAYLOAD_SCHEMA].asString(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
                }
            }

        }
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
    }
}

std::list<EZMQX::Topic> EZMQX::TopicDiscovery::query(std::string topic)
{
    if (!ctx)
    {
        throw EZMQX::Exception("Could not initialize context", EZMQX::UnKnownState);
    }

    // TODO validation check
    if (topic.empty())
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

    // mode check
    if (ctx->isStandAlone() && !ctx->isTnsEnabled())
    {
        throw EZMQX::Exception("Could not use discovery with out tns server", EZMQX::InvalidTopic);
    }

    //tns server addr check
    std::list<EZMQX::Topic> topics;

    try
    {
        validateTopic(topic, topics);
    }
    catch(const EZMQX::Exception& e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Exception: %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Unknown Exception", __func__);
        throw EZMQX::Exception("Could not query topic", EZMQX::UnKnownState);
    }

    return topics;
}