#ifndef EZMQ_X_SUBSCRIBER_H
#define EZMQ_X_SUBSCRIBER_H

#include <string>
#include <memory>
#include <list>
#include <EZMQXErrorCode.h>
#include <EZMQXTopic.h>
#include <EZMQSubscriber.h>

namespace EZMQX
{
class Subscriber;

typedef std::function<void(std::string topic, void* object)> SubCb;
typedef std::function<void(std::shared_ptr<EZMQX::Subscriber> subscriber, EZMQX::ErrorCode errCode)> SubErrCb;

class Subscriber
{
    private:
        std::list<ezmq::EZMQSubscriber> subscribers;
        EZMQX::SubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        ezmq::EZMQSubCB mSubCallback;
        ezmq::EZMQSubTopicCB mSubTopicCallback;

        // delete default ctor
        Subscriber();
        Subscriber(const  std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        // make noncopyable
        Subscriber(const Subscriber&) = delete;
        Subscriber& operator = (const Subscriber&) = delete;
    public:
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const EZMQX::Topic &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        bool isTerminated();
        void terminate();
        std::list<EZMQX::Topic> getTopic();
};

}

#endif //EZMQ_X_SUBSCRIBER_H