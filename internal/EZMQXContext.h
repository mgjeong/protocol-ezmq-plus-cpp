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
class Context
{
    private:
        friend class FakeSingletonAccessor;
        friend class Config;
        bool standAlone;
        bool tnsEnabled;
        std::mutex lock;
        std::atomic_bool initialized;
        std::atomic_bool terminated;
        std::string hostname;
        std::string hostAddr;
        std::string remoteAddr;
        std::list<std::string> topicList;
        static std::shared_ptr<EZMQX::Context> _instance;
        std::map<int, int> ports;
        std::map<int, bool> usedPorts;
        int usedIdx;
        int numOfPort;
        std::map<std::string, std::shared_ptr<AML::Representation>> amlRepDic;
        void setStandAloneMode(bool mode);
        void setHostInfo(std::string hostName, std::string hostAddr);
        void setTnsInfo(std::string remoteAddr);
        void initialize();
        void terminate();

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

};

} // namespace EZMQX

#endif //EZMQ_X_CONTEXT_H