#ifndef EXMQ_X_PUBLISHER_H
#define EXMQ_X_PUBLISHER_H

//#include <AML.h>
#include <string>
#include <memory>
#include <atomic>
#include <EZMQXTopic.h>
#include <EZMQPublisher.h>
#include <EZMQXErrorCode.h>
#include <AMLInterface.h>
#include <Representation.h>
#include <EZMQXAmlModelInfo.h>

namespace EZMQX {

class Publisher
{
    private:
        std::mutex lock;
        std::atomic_bool terminated;
        std::shared_ptr<ezmq::EZMQPublisher> pubCtx;
        std::shared_ptr<Representation> rep;
        int localPort;
        EZMQX::Topic topic;
        std::string token;

        virtual void registerTopic(const EZMQX::Topic& topic);

        // delete default ctor
        Publisher();
        Publisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);

        // make noncopyable        
        Publisher(const Publisher&) = delete;
        Publisher& operator = (const Publisher&) = delete;

    public:
        ~Publisher();
        static std::shared_ptr<EZMQX::Publisher> getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);
        void publish(const AMLObject& payload);
        bool isTerminated();
        void terminate();
};
} // namespace EZMQX

#endif //EXMQ_X_PUBLISHER_H