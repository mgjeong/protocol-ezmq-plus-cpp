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


#ifndef EZMQ_X_SUBSCRIBER_H
#define EZMQ_X_SUBSCRIBER_H

#include <string>
#include <list>
#include <queue>
#include <memory>
#include <atomic>
#include <EZMQXErrorCode.h>
#include <EZMQXTopic.h>
#include <EZMQSubscriber.h>
#include <AMLInterface.h>
#include <Representation.h>
#include <EZMQXAmlModelInfo.h>

namespace EZMQX
{
class BlockingQue;
class Context;

class Context;
/**
* @class Subscriber
* Super class for Subscriber.
*
*/
class Subscriber
{
    protected:
        friend class Context;
        std::mutex lock;
        std::atomic_bool terminated;
        EZMQX::Context* ctx;
        ezmq::EZMQSubscriber* subCtx;
        std::list<EZMQX::Topic> storedTopics;
        std::map<std::string, std::shared_ptr<AML::Representation>> repDic;
        std::string token;
        EZMQX::BlockingQue* que;
        std::thread mThread;
        bool secured;
        std::atomic_bool mTerminated;

        void handler();
        virtual void cb(const std::string &_topic, const AML::AMLObject* obj) = 0;

        void internalSubCb(std::string topic, const ezmq::EZMQMessage &event);
        void initialize(const std::string &topic, bool isHierarchical);
        void initialize(const std::list<EZMQX::Topic> &topics);
        void initialize(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey);
        void initialize(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey);

        void validateTopic(const std::string& topic);
        void verifyTopics(const std::string &topic, std::list<EZMQX::Topic> &verified, bool isHierarchical);
        void getSession(const EZMQX::Topic &topic);
        void getSession(EZMQX::Topic topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey);

        Subscriber();
        virtual ~Subscriber() = 0;
        // make noncopyable
        Subscriber(const Subscriber&) = delete;
        Subscriber& operator = (const Subscriber&) = delete;

        bool isSecured();
        bool isTerminated();
        void terminate();
        void terminateOwnResource();
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_SUBSCRIBER_H