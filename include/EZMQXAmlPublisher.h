#ifndef EXMQ_X_AML_PUBLISHER_H
#define EXMQ_X_AML_PUBLISHER_H

#include <EZMQXPublisher.h>
#include <AMLInterface.h>
#include <Representation.h>
#include <EZMQXAmlModelInfo.h>

namespace EZMQX {

class AmlPublisher : public Publisher
{
    private:
        std::shared_ptr<Representation> rep;

        // delete default ctor
        AmlPublisher();
        AmlPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);

        // make noncopyable
        AmlPublisher(const AmlPublisher&) = delete;
        AmlPublisher& operator = (const AmlPublisher&) = delete;

    public:
        ~AmlPublisher();
        static std::shared_ptr<EZMQX::AmlPublisher> getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);
        void publish(const AMLObject& payload);
};
} // namespace EZMQX

#endif //EXMQ_X_AML_PUBLISHER_H