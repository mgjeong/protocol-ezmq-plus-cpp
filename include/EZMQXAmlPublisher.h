/**
* @file
*
* This file contains APIs for AmlPublisher.
*/

#ifndef EXMQ_X_AML_PUBLISHER_H
#define EXMQ_X_AML_PUBLISHER_H

#include <EZMQXPublisher.h>
#include <AMLInterface.h>
#include <Representation.h>
#include <EZMQXAmlModelInfo.h>

namespace EZMQX {
/**
* @class AmlPublisher
* This class Contains the APIs for publish AMLObject.
*
*/
class AmlPublisher : public Publisher
{
    private:
        std::shared_ptr<AML::Representation> rep;

        // delete default ctor
        AmlPublisher();
        AmlPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);

        // make noncopyable
        AmlPublisher(const AmlPublisher&) = delete;
        AmlPublisher& operator = (const AmlPublisher&) = delete;

    public:
        /**
        * Destructor of AmlPublisher.
        */
        ~AmlPublisher();

        /**
        * Get instance of AmlPublisher class.
        *
        * @param topic Topic to be used for publish.
        * @param infoType enum for data model info, EZMQX::AmlModelId or EZMQX::AmlFilePath.
        * @param amlModelInfo  string of modelId of filePath.
        * @param optionalPort Port to be used for publish when StandAloneMode.
        *
        * @throws EZMQX::Exception thrown with ErrorCode
        * @see EZMQX::Exception
        *
        * @return Instance of AmlPublisher class
        */
        static EZMQX::AmlPublisher* getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort);

        /**
        * Publish AMLObject on the socket for subscribers.
        *
        * @param payload AMLObject to be published.
        *
        * @throws EZMQX::Exception thrown with ErrorCode.
        * @see EZMQX::Exception
        *
        */
        void publish(const AML::AMLObject& payload);

        /**
        * Get instance of Topic that used on this publisher.
        *
        * @return Topic Topic instance that include topic name, data model, endpoint.
        *
        */
        EZMQX::Topic getTopic();

        /**
        * Get status of AmlPublisher instance.
        *
        * @return bool Current status of AmlPublisher instance.
        *
        */
        bool isTerminated();

        /**
        * Terminate AmlPublisher instance.
        */
        void terminate();
};
} // namespace EZMQX

#endif //EXMQ_X_AML_PUBLISHER_H