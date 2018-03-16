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

namespace EZMQX {
class FakeSingletonAccessor;
class Context
{
    private:
        friend class FakeSingletonAccessor;
        std::mutex lock;
        std::atomic_bool initialized;
        std::atomic_bool terminated;
        std::string hostname;
        std::string hostAddr;
        std::string remoteAddr;
        static std::shared_ptr<EZMQX::Context> _instance;
        std::map<int, int> ports;
        std::map<int, bool> usedPorts;
        int usedIdx;
        int numOfPort;
        std::map<std::string, std::shared_ptr<Representation>> amlRepDic;
        void initialize();
        void terminate();

        // make noncopyable        
        Context();
        Context(const Context&) = delete;
        Context& operator = (const Context&) = delete;

        // ctor for fake object
        Context(std::string fakeHostname, std::string fakeHostAddr, std::string fakeRemoteAddr, std::map<int, int> fakePorts);

    public:
        ~Context();
        static std::shared_ptr<EZMQX::Context> getInstance();
        bool isInitialized();
        bool isTerminated();
        int assignDynamicPort();
        void releaseDynamicPort(int port);
        EZMQX::Endpoint getHostEp(int port);
        std::list<std::string> addAmlRep(const std::list<std::string>& amlModelInfo);
        std::shared_ptr<Representation> getAmlRep(const std::string& amlModelId);

};

} // namespace EZMQX

#endif //EZMQ_X_CONTEXT_H