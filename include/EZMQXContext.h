#ifndef EZMQ_X_CONTEXT_H
#define EZMQ_X_CONTEXT_H

#include <map>
#include <memory>
#include <EZMQXEndpoint.h>

namespace EZMQX {

class Context
{
    private:
        EZMQX::Endpoint hostEp;
        std::shared_ptr<EZMQX::Context> instance;
        std::map<int, int> ports;
        void initialize();

        // make noncopyable        
        Context();
        Context(const Context&) = delete;
        void operator(const Context&) = delete;

    public:
        static std::shared_ptr<EZMQX::Context> getInstance();
};

} // namespace EZMQX

#endif //EZMQ_X_CONTEXT_H