#include <utility>
#include <EZMQXKeepAlive.h>
#include <EZMQXBlockingQue.h>
#include <EZMQXException.h>
#include <EZMQXRest.h>
#include <algorithm>
#include <json/writer.h>
#include <json/reader.h>
#include <EZMQXContext.h>
#include <chrono>

// Rest Option
const static std::string KEEP_ALIVE = "KEEP_ALIVE";
const static std::string UNREGISTER_TOPIC = "UNREGISTER_TOPIC";

// Rest Endpoints
static const std::string PREFIX = "/api/v1";
static const std::string TNS_KNOWN_PORT = ":48323";
static const std::string TNS_KEEP_ALIVE_PORT = ":48324";
static const std::string TNS_KEEP_ALIVE = "/tns/keepalive";
static const std::string TNS_UNREGISTER = "/tns/topic";

// Json keys
static const std::string PAYLOAD_CID = "c_id";
static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_TOPICS = "topics";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";
static const std::string RESULT_KEY = "result";
static const std::string RESULT_SUCCESS = "success";
static const std::string RESULT_DUPLICATED = "duplicated";


void EZMQX::KeepAlive::inQue(EZMQX::TaskOption opt, std::string payload)
{
    std::string restOpt;

    if (opt == EZMQX::TopicKeepAlive)
    {
        restOpt = KEEP_ALIVE;
    }
    else if (opt == EZMQX::UnregisterTopic)
    {
        restOpt = UNREGISTER_TOPIC;
    }
    else
    {
        return;
    }

    que->inQue(std::make_pair(restOpt, payload));
}

void EZMQX::KeepAlive::queHandler()
{
    while(1)
    {
        std::pair<std::string, std::string> payload;

        try
        {
            if (!que)
            {
                throw EZMQX::Exception("que is not initiated", EZMQX::UnKnownState);
            }

            que->deQue(payload);

            if (payload.first.empty() || payload.second.empty())
            {
                throw EZMQX::Exception("empty payload", EZMQX::UnKnownState);
            }

            // send Rest here
            std::string ret;

            if (payload.first.compare(KEEP_ALIVE) == 0)
            {
                EZMQX::SimpleRest rest;
                ret = rest.Post(remoteAddr + TNS_KEEP_ALIVE_PORT + PREFIX + TNS_KEEP_ALIVE, payload.second);
            }
            else if (payload.first.compare(UNREGISTER_TOPIC) == 0)
            {
                EZMQX::SimpleRest rest;
                ret = rest.Delete(remoteAddr + TNS_KNOWN_PORT + PREFIX + TNS_UNREGISTER, payload.second);
            }
            else
            {
                continue;
            }

            Json::Value root;
            Json::Reader reader;
            if (reader.parse(ret, root))
            {
                ret = root[RESULT_KEY].asString();

                if (ret.compare(RESULT_SUCCESS) == 0)
                {
                    continue;
                }
                else
                {
                    // try again
                    que->inQue(payload);
                }
            }

        }
        catch(...)
        {
            continue;
        }
    }

    return;
}

void EZMQX::KeepAlive::timerHandler()
{
    while(1)
    {
        // get list of current topic
        std::list<std::string> topicList = EZMQX::Context::getInstance()->getTopicList();
        if (!topicList.empty())
        {
            Json::Value root;
            root[PAYLOAD_TOPICS] = Json::Value(Json::arrayValue);
            // add task for rest
            for (auto itr = topicList.begin(); itr != topicList.end(); itr++)
            {
                Json::Value value;
                value[PAYLOAD_TOPIC] = *itr;
                root[PAYLOAD_TOPICS].append(value);
            }

            Json::FastWriter writer;
            std::string payload = writer.write(root);

            // queing here
            inQue(EZMQX::TopicKeepAlive, payload);
        }

        // sleep for ....
        std::this_thread::sleep_for(std::chrono::minutes(3));
    }
}

EZMQX::KeepAlive::KeepAlive(){/*DoNotUseIt*/}

EZMQX::KeepAlive::KeepAlive(std::string addr) : remoteAddr(addr)
{
    que = new EZMQX::BlockingQue();
    queThread = std::thread(&EZMQX::KeepAlive::queHandler, this);
    timerThread = std::thread(&EZMQX::KeepAlive::timerHandler, this);
}

EZMQX::KeepAlive::~KeepAlive()
{
    queThread.join();
    timerThread.join();
    delete que;
}