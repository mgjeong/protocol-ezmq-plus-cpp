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

static const std::string TNS_KNOWN_PORT = "48323";
static const std::string COLLON = ":";

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

static const std::string TOPIC_PATTERN = "(\/[a-zA-Z0-9-_*.]+)+";
static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

#ifdef __GNUC__
#define EZMQX_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

EZMQX::TopicDiscovery::TopicDiscovery() : ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create instance context not initialized", EZMQX::NotInitialized);
    }
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

void EZMQX::TopicDiscovery::verifyTopic(std::string& topic, std::list<EZMQX::Topic>& topics, bool isHierarchical)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    std::string resp;
    EZMQX::RestResponse restResp;
    // send rest
    try
    {
        std::string query = QUERY_NAME + topic + QUERY_HIERARCHICAL + (isHierarchical == true ? QUERY_TRUE : QUERY_FALSE);
        restResp = EZMQX::RestService::Get(ctx->getTnsAddr() + COLLON + TNS_KNOWN_PORT + PREFIX + TOPIC, query);
        resp = restResp.getPayload();

        if (restResp.getStatus() == EZMQX::Success)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s topic %s query successfully", __func__, topic.c_str());
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
        if (!(reader->parse(resp.c_str(), resp.c_str() + resp.size(), &root, &errors)))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object error : %s", __func__, errors.c_str());
            throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
        }
        else
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s json object parsed %s", __func__, resp.c_str());
            props = root[PAYLOAD_TOPICS];
            // access array
            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
            {
                // get Topic
                if (props[i].isMember(PAYLOAD_NAME) && props[i].isMember(PAYLOAD_DATAMODEL) && props[i].isMember(PAYLOAD_ENDPOINT))
                {
                    topics.push_back(EZMQX::Topic(props[i][PAYLOAD_NAME].asString(), props[i][PAYLOAD_DATAMODEL].asString(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
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
}

EZMQX::Topic EZMQX::TopicDiscovery::query(std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    if (!ctx)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Coould not initialize context", __func__);
        throw EZMQX::Exception("Could not initialize context", EZMQX::UnKnownState);
    }

    if (ctx->isTerminated())
    {
        throw EZMQX::Exception("context terminated", EZMQX::Terminated);
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
        verifyTopic(topic, topics, false);
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

    return topics.front();
}

std::list<EZMQX::Topic> EZMQX::TopicDiscovery::hierarchicalQuery(std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    if (!ctx)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Coould not initialize context", __func__);
        throw EZMQX::Exception("Could not initialize context", EZMQX::UnKnownState);
    }

    if (ctx->isTerminated())
    {
        throw EZMQX::Exception("context terminated", EZMQX::Terminated);
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
        verifyTopic(topic, topics, true);
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