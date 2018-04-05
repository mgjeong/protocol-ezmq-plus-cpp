#ifndef EZMQ_X_AML_SUBSCRIBER_H
#define EZMQ_X_AML_SUBSCRIBER_H

#include <EZMQXSubscriber.h>

namespace EZMQX
{
typedef std::function<void(std::string topic, const AMLObject& payload)> AmlSubCb;
typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;

class AmlSubscriber : public Subscriber
{
    private:
        EZMQX::AmlSubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        void cb(const std::string &topic, const AMLObject* obj);

        // delete default ctor
        AmlSubscriber();
        AmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        AmlSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        // make noncopyable
        AmlSubscriber(const AmlSubscriber&) = delete;
        AmlSubscriber& operator = (const AmlSubscriber&) = delete;

    public:
        ~AmlSubscriber();
        static EZMQX::AmlSubscriber* getSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        static EZMQX::AmlSubscriber* getSubscriber(const EZMQX::Topic &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        static EZMQX::AmlSubscriber* getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        bool isTerminated();
        void terminate();
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_AML_SUBSCRIBER_H