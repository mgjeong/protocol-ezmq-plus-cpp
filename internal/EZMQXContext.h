#ifndef EZMQ_X_CONTEXT_H
#define EZMQ_X_CONTEXT_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <EZMQXEndpoint.h>

namespace EZMQX {

class Context
{
    private:
        std::mutex lock;
        std::atomic_bool initialized;
        std::atomic_bool terminated;
        std::string hostname;
        std::string hostAddr;
        std::string remoteAddr;
        static std::shared_ptr<EZMQX::Context> _instance;
        std::map<int, int> ports;
        void initialize();
        void terminate();

        // make noncopyable        
        Context();
        Context(const Context&) = delete;
        Context& operator = (const Context&) = delete;

    public:
        static std::shared_ptr<EZMQX::Context> getInstance();
        bool isInitialized();
        bool isTerminated();
        EZMQX::Endpoint getHostEp(int port);

};

} // namespace EZMQX

#endif //EZMQ_X_CONTEXT_H