/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/


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
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::InvalidTopic - Topic validation fail.\n
        * EZMQX::InvalidParam - Invalid file path.\n
        * EZMQX::InvalidAmlModel - Could not parse given AML model file.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnKnownState - Unknown reason.\n
        *
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
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n.
        * EZMQX::Terminated - Stack terminated.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        *
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