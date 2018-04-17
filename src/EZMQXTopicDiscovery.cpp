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

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";
static const std::string QUERY_PARAM = "topic=";
static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";

static const std::string TOPIC_PATTERN = "(\/[a-zA-Z0-9-_*.]+)+";
static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

#ifdef __GNUC__
#define EZMQX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

EZMQX::TopicDiscovery::TopicDiscovery() : ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

EZMQX::TopicDiscovery::~TopicDiscovery()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

void EZMQX::TopicDiscovery::validateTopic(std::string& topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    std::string tmp = topic;

    // simple grammer check
    if (tmp.front() != SLASH || tmp.back() == SLASH || tmp.find(DOUBLE_SLASH) != std::string::npos)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

    if (tmp.find(TOPIC_WILD_CARD) != std::string::npos && tmp.find(TOPIC_WILD_PATTERNN) == std::string::npos)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }

//Regex support is supported from  gcc-4.9 and higher
#if defined(EZMQX_GCC_VERSION) && EZMQX_GCC_VERSION >= 40900
    std::regex pattern(TOPIC_PATTERN);

    if (!std::regex_match(tmp, pattern))
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
#endif
}

void EZMQX::TopicDiscovery::verifyTopic(std::string& topic, std::list<EZMQX::Topic>& topics)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    std::string tmp;
    // send rest
    try
    {
        EZMQX::SimpleRest rest;
        tmp = rest.Get(ctx->getTnsAddr() + PREFIX + TOPIC, QUERY_PARAM + topic);
    }
    catch (...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest get request", __func__);
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
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object", __func__);
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
        EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object", __func__);
        throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
    }
}

std::list<EZMQX::Topic> EZMQX::TopicDiscovery::query(std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    if (!ctx)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Coould not initialize context", __func__);
        throw EZMQX::Exception("Could not initialize context", EZMQX::UnKnownState);
    }

    // TODO validation check
    if (topic.empty())
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Invalid topic %s", __func__, topic);
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
    else
    {
        validateTopic(topic);
    }

    // tns check
    if (!ctx->isTnsEnabled())
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not use discovery without tns server", __func__);
        throw EZMQX::Exception("Could not use discovery with out tns server", EZMQX::TnsNotAvailable);
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