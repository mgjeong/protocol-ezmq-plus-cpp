#ifndef EZMQ_X_XML_SUBSCRIBER_H
#define EZMQ_X_XML_SUBSCRIBER_H

#include <EZMQXSubscriber.h>

namespace EZMQX
{
typedef std::function<void(std::string topic, const std::string& payload)> XmlSubCb;
typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;

class XmlSubscriber : public Subscriber
{
    private:
        EZMQX::XmlSubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        void initialize(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        void cb(const std::string &topic, const AMLObject* obj);

        // delete default ctor
        XmlSubscriber();
        XmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        XmlSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        // make noncopyable
        XmlSubscriber(const XmlSubscriber&) = delete;
        XmlSubscriber& operator = (const XmlSubscriber&) = delete;

    public:
        ~XmlSubscriber();
        static std::shared_ptr<EZMQX::XmlSubscriber> getSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::XmlSubscriber> getSubscriber(const EZMQX::Topic &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::XmlSubscriber> getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        bool isTerminated();
        void terminate();
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_XML_SUBSCRIBER_H