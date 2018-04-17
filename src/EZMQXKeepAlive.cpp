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
#include <EZMQXLogger.h>

#define TAG "EZMQXKeepAlive"

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
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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

    EZMQX_LOG_V(DEBUG, TAG, "%s inQue %s %s ", __func__, (opt == EZMQX::TopicKeepAlive ? KEEP_ALIVE : UNREGISTER_TOPIC), payload);
    que->inQue(std::make_pair(restOpt, payload));
}

void EZMQX::KeepAlive::queHandler()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    while(1)
    {
        std::pair<std::string, std::string> payload;

        try
        {
            if (!que)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s que is not initiated", __func__);
                throw EZMQX::Exception("que is not initiated", EZMQX::UnKnownState);
            }

            que->deQue(payload);

            if (que->isTerminated())
            {
                return;
            }

            if (payload.first.empty() || payload.second.empty())
            {
                EZMQX_LOG_V(ERROR, TAG, "%s empty payload", __func__);
                throw EZMQX::Exception("empty payload", EZMQX::UnKnownState);
            }

            // send Rest here
            std::string ret;

            if (payload.first.compare(KEEP_ALIVE) == 0)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request %s", __func__, remoteAddr + TNS_KEEP_ALIVE_PORT + PREFIX + TNS_KEEP_ALIVE, payload.second);
                EZMQX::SimpleRest rest;
                ret = rest.Post(remoteAddr + TNS_KEEP_ALIVE_PORT + PREFIX + TNS_KEEP_ALIVE, payload.second);
                EZMQX_LOG_V(DEBUG, TAG, "%s Rest Result \n %s \n", __func__, ret);
            }
            else if (payload.first.compare(UNREGISTER_TOPIC) == 0)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request %s", __func__, remoteAddr + TNS_KNOWN_PORT + PREFIX + TNS_UNREGISTER, payload.second);
                EZMQX::SimpleRest rest;
                ret = rest.Delete(remoteAddr + TNS_KNOWN_PORT + PREFIX + TNS_UNREGISTER, payload.second);
                EZMQX_LOG_V(DEBUG, TAG, "%s Rest Result \n %s \n", __func__, ret);
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
                    EZMQX_LOG_V(DEBUG, TAG, "%s Rest Result is Success", __func__);
                    continue;
                }
                else
                {
                    // try again
                    EZMQX_LOG_V(DEBUG, TAG, "%s Rest Result is %s, try again", __func__, ret);
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
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    while(1)
    {
        if (timerIsTerminate.load())
        {
            return;
        }
        std::unique_lock<std::mutex> timerLock(timerMutex);
        // get list of current topic
        std::list<std::string> topicList = EZMQX::Context::getInstance()->getTopicList();
        if (!topicList.empty())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Build KeepAlive Rest payload", __func__);
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
            EZMQX_LOG_V(DEBUG, TAG, "%s Payload: %s", __func__, payload);

            // queing here
            inQue(EZMQX::TopicKeepAlive, payload);
        }

        // sleep for ....
        timerCond.wait_for(timerLock, std::chrono::minutes(3));
        //std::this_thread::sleep_for(std::chrono::minutes(3));
    }
}

void EZMQX::KeepAlive::stopTimer()
{
    // push to terminate
    timerIsTerminate.store(true);
    timerCond.notify_all();
    return;
}

EZMQX::KeepAlive::KeepAlive(){/*DoNotUseIt*/}

EZMQX::KeepAlive::KeepAlive(std::string addr) : remoteAddr(addr), timerIsTerminate(false), timerCond()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    que = new EZMQX::BlockingQue();
    queThread = std::thread(&EZMQX::KeepAlive::queHandler, this);
    timerThread = std::thread(&EZMQX::KeepAlive::timerHandler, this);
}

EZMQX::KeepAlive::~KeepAlive()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    que->stop();
    queThread.join();
    EZMQX_LOG_V(DEBUG, TAG, "%s que thread stoped", __func__);
    stopTimer();
    timerThread.join();
    EZMQX_LOG_V(DEBUG, TAG, "%s timer thread stoped", __func__);
    delete que;
}