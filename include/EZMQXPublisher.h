#ifndef EXMQ_X_PUBLISHER_H
#define EXMQ_X_PUBLISHER_H

#include <string>
#include <memory>
#include <atomic>
#include <EZMQXTopic.h>
#include <EZMQPublisher.h>
#include <EZMQXErrorCode.h>

namespace EZMQX {

class Publisher
{
    protected:
        std::mutex lock;
        std::atomic_bool terminated;
        ezmq::EZMQPublisher* pubCtx;
        int localPort;
        EZMQX::Topic topic;
        std::string token;

        virtual void registerTopic(EZMQX::Topic& topic);

        // delete default ctor
        Publisher();
        Publisher(int optionalPort);

        // make noncopyable        
        Publisher(const Publisher&) = delete;
        Publisher& operator = (const Publisher&) = delete;

        bool isTerminated();
        void terminate();

    public:
        ~Publisher();
};
} // namespace EZMQX

#endif //EXMQ_X_PUBLISHER_H