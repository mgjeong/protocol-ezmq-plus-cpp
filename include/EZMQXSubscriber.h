#ifndef EZMQ_X_SUBSCRIBER_H
#define EZMQ_X_SUBSCRIBER_H

#include <string>
#include <memory>
#include <list>
#include <EZMQXErrorCode.h>
#include <EZMQXContext.h>
#include <EZMQXTopic.h>

namespace EZMQX
{

typedef std::function<void(std::string topic, void* object)> EZMQXSubCB;
typedef std::function<void(std::shared_ptr<EZMQX::Subscriber> subscriber, ErrorCode errCode)> EZMQXErrCB;

class Subscriber
{
    private:
        std::shared_ptr<EZMQX::Context> ctx;

        void initialize();

        // delete default ctor
        Subscriber();
        Subscriber(std::string topic, EZMQXSubCB subCb, EZMQXErrCB errCb);
        Subscriber(std::list<std::string> topics, EZMQXSubCB subCb, EZMQXErrCB errCb);
        // make noncopyable
        Subscriber(const Subscriber&) = delete;
        void operator(const Subscriber&) = delete;
    public:
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(std::string topic, EZMQXSubCB subCb, EZMQXErrCB errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(std::list<std::string> topics, EZMQXSubCB subCb, EZMQXErrCB errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(EZMQX::Topic topic, EZMQXSubCB subCb, EZMQXErrCB errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(std::list<EZMQX::Topic> topics, EZMQXSubCB subCb, EZMQXErrCB errCb);
        void terminate();
};

}

#endif //EZMQ_X_SUBSCRIBER_H