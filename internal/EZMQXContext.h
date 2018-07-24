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


#ifndef EZMQ_X_CONTEXT_H
#define EZMQ_X_CONTEXT_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <list>
#include <memory>
#include <EZMQXEndpoint.h>
#include <Representation.h>
#include <AMLException.h>
#include <EZMQAPI.h>

namespace EZMQX {
class FakeSingletonAccessor;
class Config;
class KeepAlive;
class Publisher;
class Subscriber;
class Context
{
    private:
        friend class FakeSingletonAccessor;
        friend class Config;
        KeepAlive *keepAlive;
        bool standAlone;
        bool tnsEnabled;
        std::mutex lock;
        std::atomic_bool initialized;
        std::atomic_bool terminated;
        std::string hostname;
        std::string hostAddr;
        std::string remoteAddr;
        std::list<std::string> topicList;
        std::atomic_int interval;
        std::string imageName;
        static std::shared_ptr<EZMQX::Context> _instance;
        std::map<int, int> ports;
        std::map<int, bool> usedPorts;
        int usedIdx;
        int numOfPort;
        std::list<EZMQX::Publisher*> publishers;
        std::list<EZMQX::Subscriber*> subscribers;
        std::map<std::string, std::shared_ptr<AML::Representation>> amlRepDic;
        void setStandAloneMode(bool mode, const std::string& tnsConfPathRef);
        void setHostInfo(std::string hostName, std::string hostAddr);
        void setTnsInfo(std::string remoteAddr,const std::string& tnsConfPathRef);
        void initialize(const std::string& tnsConfPathRef);
        void terminate();
        //
        void getImageName(const std::string& tnsConfPathRef);        
        //

        // make noncopyable
        Context();
        Context(const Context&) = delete;
        Context& operator = (const Context&) = delete;

    public:
        ~Context();
        static EZMQX::Context* getInstance();
        bool isInitialized();
        bool isTerminated();
        bool isStandAlone();
        bool isTnsEnabled();
        std::string getTnsAddr();
        int assignDynamicPort();
        void releaseDynamicPort(int port);
        EZMQX::Endpoint getHostEp(int port);
        std::list<std::string> addAmlRep(const std::list<std::string>& amlModelInfo);
        std::shared_ptr<AML::Representation> getAmlRep(const std::string& amlModelId);
        void insertTopic(std::string topic);
        void deleteTopic(std::string topic);
        std::list<std::string> getTopicList();
        int updateKeepAliveInterval(int keepAliveInterval);
        int getKeepAliveInterval();
        void registerPublisher(EZMQX::Publisher* publisher);
        void unregisterPublisher(EZMQX::Publisher* publisher);
        void registerSubscriber(EZMQX::Subscriber* subscriber);
        void unregisterSubscriber(EZMQX::Subscriber* subscriber);
};

} // namespace EZMQX

#endif //EZMQ_X_CONTEXT_H