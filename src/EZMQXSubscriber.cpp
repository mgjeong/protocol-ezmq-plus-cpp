/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

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
#include <chrono>
#include <regex>
#include <EZMQErrorCodes.h>

#define TAG "EZMQXSubscriber"
#define SLASH '/'
#define DOUBLE_SLASH "//"

static const std::string TOPIC_PATTERN = "(\\/[a-zA-Z0-9-_*.]+)+";

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
static const std::string PAYLOAD_SECURED = "secured";

static const std::string TOPIC_WILD_CARD = "*";
static const std::string TOPIC_WILD_PATTERNN = "/*/";

EZMQX::Subscriber::Subscriber() : terminated(false), ctx(EZMQX::Context::getInstance()), subCtx(nullptr), token(""), que(new EZMQX::BlockingQue()), mThread(std::thread(&EZMQX::Subscriber::handler, this)), secured(false), mTerminated(false)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

EZMQX::Subscriber::~Subscriber()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Subscriber::initialize(const std::string &topic, bool isHierarchical)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    validateTopic(topic);

    std::list<EZMQX::Topic> verified;

    if (ctx->isTnsEnabled())
    {
        verifyTopics(topic, verified, isHierarchical);

        if (verified.empty())
        {
            throw EZMQX::Exception("Could not find matched topic", EZMQX::NoTopicMatched);
        }
    }
    else
    {
        throw EZMQX::Exception("TNS not available", EZMQX::TnsNotAvailable);
    }

    for (std::list<EZMQX::Topic>::const_iterator itr = verified.cbegin(); itr != verified.cend(); itr++)
    {
        if ((*itr).isSecured())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s topic is secured!!!", __func__);
            throw EZMQX::Exception("topic is secured", EZMQX::InvalidParam);
        }
    }

    try
    {
        Subscriber::initialize(verified);

        // register on CTX
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

void EZMQX::Subscriber::getSession(const EZMQX::Topic &topic)
{
    std::string empty="";
    return getSession(topic, empty, empty, empty);
}

void EZMQX::Subscriber::getSession(EZMQX::Topic topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey)
{
    EZMQX::Endpoint ep = topic.getEndpoint();

    try
    {
        if (subCtx == nullptr)
        {
            // create new one
            subCtx = new ezmq::EZMQSubscriber(ep.getAddr(), ep.getPort(),
            std::bind(&EZMQX::Subscriber::internalSubCb, this, "", std::placeholders::_1),
            std::bind(&EZMQX::Subscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));

            if (!subCtx)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Could not connect with endpoint %s ", __func__, ep.toString().c_str());
                throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::SessionUnavailable);
            }

            if (serverPublicKey.length() != 0)
            {
                if (subCtx->setServerPublicKey(serverPublicKey) != ezmq::EZMQ_OK)
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s Could not set server public key %s ", __func__, ep.toString().c_str());
                    throw EZMQX::Exception("Could not set server public key " + ep.toString(), EZMQX::SessionUnavailable);
                }
            }

            if (clientPublicKey.length() != 0 && clientSecretKey.length() != 0)
            {
                if (subCtx->setClientKeys(clientSecretKey, clientPublicKey) != ezmq::EZMQ_OK)
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s Could not set client key %s ", __func__, ep.toString().c_str());
                    throw EZMQX::Exception("Could not set client key " + ep.toString(), EZMQX::SessionUnavailable);
                }
            }

            ezmq::EZMQErrorCode ret = subCtx->start();

            if (ezmq::EZMQ_OK != ret)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Could not start session with endpoint %s ", __func__, ep.toString().c_str());
                throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::SessionUnavailable);
            }

            ret = subCtx->subscribe(topic.getName());

            if (ezmq::EZMQ_OK != ret)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Could not subscribe with endpoint %s ", __func__, ep.toString().c_str());
                throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::SessionUnavailable);
            }
        }
        else
        {
            // append session
            if (serverPublicKey.length() != 0)
            {
                if (subCtx->setServerPublicKey(serverPublicKey) != ezmq::EZMQ_OK)
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s Could not set server public key %s ", __func__, ep.toString().c_str());
                    throw EZMQX::Exception("Could not set server public key " + ep.toString(), EZMQX::SessionUnavailable);
                }
            }

            //EZMQErrorCode subscribe(const std::string &ip, const int &port, std::string topic);
            ezmq::EZMQErrorCode ret = subCtx->subscribe(ep.getAddr(), ep.getPort(), topic.getName());

            if (ezmq::EZMQ_OK != ret)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Could not subscribe with endpoint %s ", __func__, ep.toString().c_str());
                throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::SessionUnavailable);
            }
        }
    }
    catch(const EZMQX::Exception &e)
    {
        if (subCtx)
        {
            delete subCtx;
            subCtx = nullptr;
        }

        throw e;
    }
    catch(...)
    {
        if (subCtx)
        {
            delete subCtx;
            subCtx = nullptr;
        }

        throw EZMQX::Exception("Could not connect endpoint " + ep.toString(), EZMQX::UnKnownState);
    }

    return;
}


void EZMQX::Subscriber::initialize(const std::list<EZMQX::Topic> &topics)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getName();

        // find Aml rep
        try
        {
            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getDatamodel())));
        }
        catch(const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            throw EZMQX::Exception("Could not found Aml Rep", EZMQX::UnknownAmlModel);
        }

        try
        {
            getSession(topic);
        }
        catch(const EZMQX::Exception &e)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s exception: %s", __func__, e.what());

            // clear subscriber
            if (subCtx)
            {
                delete subCtx;
                subCtx = nullptr;
            }

            throw e;
        }

        storedTopics.push_back(topic);
        EZMQX_LOG_V(DEBUG, TAG, "%s Topic: %s Model_Id: %s Endpoint: %s ", __func__, topic.getName().c_str(), topic.getDatamodel().c_str(), topic.getEndpoint().toString().c_str());
    }

    // register on CTX

    return;
}

void EZMQX::Subscriber::initialize(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    EZMQX::Topic _topic = topic;
    const std::string &topic_str = _topic.getName();

    // find Aml rep
    try
    {
        repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(_topic.getDatamodel())));
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not found Aml Rep", EZMQX::UnknownAmlModel);
    }

    try
    {
        getSession(topic, serverPublicKey, clientPublicKey, clientSecretKey);
    }
    catch(const EZMQX::Exception &e)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s exception: %s", __func__, e.what());

        // clear subscriber
        if (subCtx)
        {
            delete subCtx;
            subCtx = nullptr;
        }

        throw e;
    }

    storedTopics.push_back(topic);
    this->secured = true;
    EZMQX_LOG_V(DEBUG, TAG, "%s Topic: %s Model_Id: %s Endpoint: %s ", __func__, _topic.getName().c_str(), _topic.getDatamodel().c_str(), _topic.getEndpoint().toString().c_str());

    return;

}

void EZMQX::Subscriber::initialize(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!ctx->isInitialized())
    {
        throw EZMQX::Exception("Could not create Subscriber context not initialized", EZMQX::NotInitialized);
    }

    for (std::map<EZMQX::Topic, std::string>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = itr->first;
        const std::string &topic_str = topic.getName();

        // find Aml rep
        try
        {
            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getDatamodel())));
        }
        catch(const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            throw EZMQX::Exception("Could not found Aml Rep", EZMQX::UnknownAmlModel);
        }

        try
        {
            std::string serverPublicKey = itr->second;
            getSession(topic, serverPublicKey, clientPublicKey, clientSecretKey);
        }
        catch(const EZMQX::Exception &e)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s exception: %s", __func__, e.what());

            // clear subscriber
            if (subCtx)
            {
                delete subCtx;
                subCtx = nullptr;
            }

            throw e;
        }

        storedTopics.push_back(topic);
        this->secured = true;
        EZMQX_LOG_V(DEBUG, TAG, "%s Topic: %s Model_Id: %s Endpoint: %s ", __func__, topic.getName().c_str(), topic.getDatamodel().c_str(), topic.getEndpoint().toString().c_str());
    }

    return;

}

void EZMQX::Subscriber::handler()
{
    while(1)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        std::pair<std::string, std::string> payload;
        AML::AMLObject *obj = nullptr;

        if (!que || que->isTerminated())
        {
            mTerminated.store(true);
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

            EZMQX_LOG_V(DEBUG, TAG, "%s try deque", __func__);
            que->deQue(payload);

            EZMQX_LOG_V(DEBUG, TAG, "%s deque successfully", __func__);

            if (que->isTerminated())
            {
                mTerminated.store(true);
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
                obj = itr->second->ByteToData(payload.second);

                if (!obj)
                {
                    throw EZMQX::Exception("Could not convert byte to AMLObject", EZMQX::UnKnownState);
                }

                // call subCb
                cb(payload.first, obj);

                // delete AmlObject
                if (obj)
                {
                    delete obj;
                    obj = nullptr;
                }
            }
        }
        catch(const std::exception &e)
        {
            // delete AmlObject
            if (obj)
            {
                delete obj;
                obj = nullptr;
            }

            // call errCb
            EZMQX_LOG_V(DEBUG, TAG, "%s exception on subscriber callback thread: %s", __func__, e.what());
            AML::AMLObject *obj = nullptr;
            cb(payload.first, obj);
        }
        catch(...)
        {
            if (obj)
            {
                delete obj;
                obj = nullptr;
            }
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

    std::regex pattern(TOPIC_PATTERN);

    if (!std::regex_match(tmp, pattern))
    {
        throw EZMQX::Exception("Invalid topic", EZMQX::InvalidTopic);
    }
}

void EZMQX::Subscriber::verifyTopics(const std::string &topic, std::list<EZMQX::Topic> &verified, bool isHierarchical)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // send rest
    std::string tmp;
    EZMQX::RestResponse restResp;
    try
    {
        std::string query = QUERY_NAME + topic + QUERY_HIERARCHICAL + (isHierarchical == true ? QUERY_TRUE : QUERY_FALSE);
        restResp = EZMQX::RestService::Get(ctx->getTnsAddr() + PREFIX + TOPIC, query);

        if (restResp.getStatus() == EZMQX::Success)
        {
            tmp = restResp.getPayload();
            EZMQX_LOG_V(DEBUG, TAG, "%s topic %s query successfully", __func__, topic.c_str());
            EZMQX_LOG_V(DEBUG, TAG, "%s", restResp.getPayload().c_str());
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
        if (!(reader->parse(tmp.c_str(), tmp.c_str() + tmp.size(), &root, &errors)))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json object error : %s", __func__, errors.c_str());
            throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s json object parsed", __func__);
            props = root[PAYLOAD_TOPICS];
            EZMQX_LOG_V(ERROR, TAG, "%s props parsed", __func__);
            // access array
            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
            {
                // get Topic
                if (props[i].isMember(PAYLOAD_NAME) && props[i].isMember(PAYLOAD_DATAMODEL) && props[i].isMember(PAYLOAD_ENDPOINT) && props[i].isMember(PAYLOAD_SECURED))
                {
                    verified.push_back(EZMQX::Topic(props[i][PAYLOAD_NAME].asString(), props[i][PAYLOAD_DATAMODEL].asString(), props[i][PAYLOAD_SECURED].asBool(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
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

    return;
}

bool EZMQX::Subscriber::isSecured()
{
    return this->secured;
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
            // release resource

            EZMQX_LOG_V(DEBUG, TAG, "%s try stop callback thread", __func__);
            while(mTerminated.load() == false)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s try stop", __func__);
                que->stop();
                EZMQX_LOG_V(DEBUG, TAG, "%s stop successfully", __func__);

                EZMQX_LOG_V(DEBUG, TAG, "%s wait thread become to joinable", __func__);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            EZMQX_LOG_V(DEBUG, TAG, "%s try join", __func__);
            mThread.join();
            EZMQX_LOG_V(DEBUG, TAG, "%s join successfully", __func__);

            delete que;

            EZMQX_LOG_V(DEBUG, TAG, "%s try terminate ezmq subscriber", __func__);
            if (subCtx)
            {
                delete subCtx;
                subCtx = nullptr;
            }

            // unregister from CTX
            if (!ctx->isTerminated())
            {
                ctx->unregisterSubscriber(this);
            }

            terminated.store(true);
        }
        else
        {
            EZMQX_LOG_V(INFO, TAG, "%s Subscriber already terminated", __func__);
            return;
        }
    }
    // mutex unlock
    return;
}

void EZMQX::Subscriber::terminateOwnResource()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // release resource

            EZMQX_LOG_V(DEBUG, TAG, "%s try stop callback thread", __func__);

            while(mTerminated.load() == false)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s try stop", __func__);
                que->stop();
                EZMQX_LOG_V(DEBUG, TAG, "%s stop successfully", __func__);

                EZMQX_LOG_V(DEBUG, TAG, "%s wait thread become to joinable", __func__);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }


            EZMQX_LOG_V(DEBUG, TAG, "%s try join", __func__);
            mThread.join();
            EZMQX_LOG_V(DEBUG, TAG, "%s join successfully", __func__);

            delete que;

            EZMQX_LOG_V(DEBUG, TAG, "%s try terminate ezmq subscriber", __func__);
            if (subCtx)
            {
                delete subCtx;
                subCtx = nullptr;
            }

            terminated.store(true);
        }
        else
        {
            EZMQX_LOG_V(INFO, TAG, "%s Subscriber already terminated", __func__);
            return;
        }
    }
    // mutex unlock
    return;
}

std::list<EZMQX::Topic> EZMQX::Subscriber::getTopics()
{
    return storedTopics;
}