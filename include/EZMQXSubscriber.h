#ifndef EZMQ_X_SUBSCRIBER_H
#define EZMQ_X_SUBSCRIBER_H

#include <string>
#include <list>
#include <memory>
#include <atomic>
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
        std::mutex lock;
        std::atomic_bool terminated;
        std::list<std::shared_ptr<ezmq::EZMQSubscriber>> subscribers;
        std::map<std::string, std::pair<SubCb, SubErrCb>> callbacks;
        std::string token;
        EZMQX::SubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        ezmq::EZMQSubCB mSubCallback;
        ezmq::EZMQSubTopicCB mSubTopicCallback;
        void initialize(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        virtual void verifyTopics(const std::list<std::string> &topics, std::list<EZMQX::Topic> &verified);
        virtual void verifyTopics(const std::list<EZMQX::Topic> &topics);

        // delete default ctor
        Subscriber();
        Subscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
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
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_SUBSCRIBER_H