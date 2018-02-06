#ifndef EXMQ_X_PUBLISHER_H
#define EXMQ_X_PUBLISHER_H

//#include <AML.h>
#include <string>
#include <memory>
#include <atomic>
#include <EZMQXTopic.h>
#include <EZMQPublisher.h>
#include <EZMQXErrorCode.h>

namespace EZMQX {
class Publisher;

typedef std::function<void(std::shared_ptr<EZMQX::Publisher> publisher, EZMQX::ErrorCode errCode)> PubErrCb;

class Publisher
{
    private:
        std::mutex lock;
        std::atomic_bool terminated;
        std::shared_ptr<ezmq::EZMQPublisher> pubCtx;
        EZMQX::Topic topic;
        EZMQX::PubErrCb mCallback;
        ezmq::EZMQStartCB mStartCallback;
        ezmq::EZMQStopCB mStopCallback;
        ezmq::EZMQErrorCB mErrorCallback;

        // delete default ctor
        Publisher();
        Publisher(const std::string &topic, const std::string &schema, const EZMQX::PubErrCb &errCb);

        // make noncopyable        
        Publisher(const Publisher&) = delete;
        Publisher& operator = (const Publisher&) = delete;

    public:
        static std::shared_ptr<EZMQX::Publisher> getPublisher(const std::string &topic, const std::string &schema, const EZMQX::PubErrCb &errCb);
        void publish(void* object);
        bool isTerminated();
        void terminate();
};
} // namespace EZMQX

#endif //EXMQ_X_PUBLISHER_H