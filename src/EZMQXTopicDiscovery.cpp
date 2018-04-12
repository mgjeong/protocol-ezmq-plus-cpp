#include <list>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>
#include <EZMQXContext.h>
#include <EZMQXRest.h>
#include <json/reader.h>
#include <EZMQXLogger.h>
#include <regex>

#define TAG "EZMQXTopicDiscovery"
#define SLASH '/'
#define DOUBLE_SLASH "//"
#define START_POS 0
#define OFFSET 1

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string QUERY_PARAM = "topic=";
static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";

static const std::string TOPIC_PATTERN = "(\/[a-zA-Z0-9-_*.]+)+";


#ifdef __GNUC__
#define EZMQX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

EZMQX::TopicDiscovery::TopicDiscovery() : ctx(EZMQX::Context::getInstance()){}

EZMQX::TopicDiscovery::~TopicDiscovery(){}

void EZMQX::TopicDiscovery::validateTopic(std::string& topic)
{
    std::string tmp = topic;

    // simple grammer check
    if (tmp.front() != SLASH || tmp.back() != SLASH || tmp.find(DOUBLE_SLASH) != std::string::npos)
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

//Regex support is supported from  gcc-4.9 and higher
#if defined(EZMQX_GCC_VERSION) && EZMQX_GCC_VERSION >= 40900
    std::regex pattern(TOPIC_PATTERN);

    // remove last slash
    tmp = tmp.substr(START_POS, tmp.length() - OFFSET);
    if (!std::regex_match(tmp, pattern))
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
#endif
}

void EZMQX::TopicDiscovery::verifyTopic(std::string& topic, std::list<EZMQX::Topic>& topics)
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
    else
    {
        validateTopic(topic);
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
        verifyTopic(topic, topics);
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