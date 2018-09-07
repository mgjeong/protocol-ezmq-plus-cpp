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
* This file provide APIs for XmlSubscriber.
*/

#ifndef EZMQ_X_XML_SUBSCRIBER_H
#define EZMQ_X_XML_SUBSCRIBER_H

#include <EZMQXSubscriber.h>

namespace EZMQX
{
/**
* Callback to get XML formatted string with topic.
*/
typedef std::function<void(std::string topic, const std::string& payload)> XmlSubCb;

/**
* Callback to get error code with topic.
*/
typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;


/**
* @class XmlSubscriber
* This class Contains the APIs that subscribe xml formatted data with topic or list of topic.
*
*/
class XmlSubscriber : public Subscriber
{
    protected:
        EZMQX::XmlSubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        void cb(const std::string &topic, const AML::AMLObject* obj);

        // delete default ctor
        XmlSubscriber() = delete;
        XmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        XmlSubscriber(const std::string &topic, bool isHierarchical, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        XmlSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);
        XmlSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        // make noncopyable
        XmlSubscriber(const XmlSubscriber&) = delete;
        XmlSubscriber& operator = (const XmlSubscriber&) = delete;

    public:
        ~XmlSubscriber();

        /**
        * Get instance of XmlSubscriber class.
        * @note This api will query to tns server with given topic and Hierarchical option.
        *
        * @param topic Topic to be subscribed.
        * @param isHierarchical Option for whether to use Hierarchical query option when query to tns.
        * @param subCb Callback to get Xml formatted string data on given topic.
        * @param errCb Error callback to get error code with topic information.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidTopic - Topic validation fail or secured topic passed.\n
        * EZMQX::TnsNotAvailable - Tns service not available, use DockerMode or set Tns information for StandAloneMode.\n
        * EZMQX::RestError - Rest error, see detail on contained message.\n
        * EZMQX::NoTopicMatched - Could not find matched topic on tns service with given topic.\n
        * EZMQX::SessionUnavailable - Could not connect with given endpoint.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return instance of XmlSubscriber class.
        */
        static EZMQX::XmlSubscriber* getSubscriber(const std::string &topic, bool isHierarchical, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        /**
        * Get instance of XmlSubscriber class.
        *
        * @param topic Instance of topic class to be Subscribed.
        * @param subCb Callback to get Xml formatted string data on given topic.
        * @param errCb Error callback to get error code with topic information.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidTopic - Secured topic passed.\n
        * EZMQX::SessionUnavailable - Could not connect with given endpoint.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return instance of XmlSubscriber class.
        */
        static EZMQX::XmlSubscriber* getSubscriber(const EZMQX::Topic &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        /**
        * Get instance of XmlSubscriber class.
        *
        * @param topics List of topic class instance to be Subscribed.
        * @param subCb Callback to get Xml formatted string data on given topic.
        * @param errCb Error callback to get error code with topic information.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidTopic - Secured topic passed.\n
        * EZMQX::SessionUnavailable - Could not connect with given endpoint.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return instance of XmlSubscriber class.
        */
        static EZMQX::XmlSubscriber* getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        /**
        * Get instance of XmlSubscriber class.
        *
        * @param topic Instance of topic class to be Subscribed.
        * @param serverPublicKey public key for server(publisher) that related with given topic.
        * @param clientPublicKey public key for client(subscriber) that shared with given topic's owner.
        * @param clientSecretKey secret key for client(subscriber) that pair of given clientPublickey
        * @param subCb Callback to get AMLObject data on given topic.
        * @param errCb errCb Error callback to get error code with topic information.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidTopic - Unsecured topic passed.\n
        * EZMQX::SessionUnavailable - Could not connect with given endpoint.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return instance of XmlSubscriber class.
        */
        static EZMQX::XmlSubscriber* getSecuredSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        /**
        * Get instance of XmlSubscriber class.
        *
        * @param topics Map of Topic and public keys.
        * @param clientPublicKey public key for client(subscriber) that shared with given topic's owner.
        * @param clientSecretKey secret key for client(subscriber) that pair of given clientPublickey
        * @param subCb Callback to get AMLObject data on given topic.
        * @param errCb errCb Error callback to get error code with topic information.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidTopic - Unsecured topic passed.\n
        * EZMQX::SessionUnavailable - Could not connect with given endpoint.\n
        * EZMQX::UnknownAmlModel - Could not find Aml Model.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return instance of XmlSubscriber class.
        */
        static EZMQX::XmlSubscriber* getSecuredSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb);

        /**
        * Return true if subscriber is secured
        *
        * @return bool Return true if subscriber is secured
        *
        */
        bool isSecured();

        /**
        * Get status of XmlSubscriber instance.
        *
        * @return bool Current status of XmlSubscriber instance.
        *
        */
        bool isTerminated();

        /**
        * Terminate XmlSubscriber instance.
        */
        void terminate();

        /**
        * Get list of Topic instance that subscribed.
        *
        * @return list of Topic instance that subscribed.
        *
        */
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_XML_SUBSCRIBER_H