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

#include <EZMQXXMLSubscriber.h>
#include <EZMQXSubscriber.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>
#include <EZMQXRest.h>
#include <iostream>
#include <json/reader.h>
#include <EZMQXBlockingQue.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXXmlSubscriber"

EZMQX::XmlSubscriber::~XmlSubscriber()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

EZMQX::XmlSubscriber::XmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    try
    {
        Subscriber::initialize(topics);
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

EZMQX::XmlSubscriber::XmlSubscriber(const std::string &topic, bool isHierarchical, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    try
    {
        Subscriber::initialize(topic, isHierarchical);
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

EZMQX::XmlSubscriber::XmlSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    try
    {
        Subscriber::initialize(topic, serverPublicKey, clientPublicKey, clientSecretKey);
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

EZMQX::XmlSubscriber::XmlSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    try
    {
        Subscriber::initialize(topics, clientPublicKey, clientSecretKey);
    }
    catch(const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

void EZMQX::XmlSubscriber::cb(const std::string &_topic, const AML::AMLObject *obj)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (ctx->isTerminated())
    {
        terminate();
        throw EZMQX::Exception("Subscriber terminated", EZMQX::Terminated);
    }

    bool isError = false;
    std::string xml;
    if (!_topic.empty() && obj != NULL)
    {
        try
        {
            auto itr = repDic.find(_topic);
            if (itr == repDic.end())
            {
                //call error cb
                isError = true;
            }

            if (!isError)
            {
                xml = itr->second->DataToAml(*obj);
                if (xml.empty())
                {
                    // call error cb
                    isError = true;
                }
            }
        }
        catch (...)
        {
            isError = true;
        }
    }

    if (isError)
    {
        // call errCb
        mSubErrCb(_topic, _topic.empty() ? UnknownTopic : BrokenPayload);
    }
    else
    {
        // call subCb
        mSubCb(_topic, xml);
    }

    return;
}

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSubscriber(const std::string &topic, bool isHierarchical, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topic, isHierarchical, subCb, errCb);
    return subInstance;
}

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    std::list<EZMQX::Topic> topics(1, topic);
    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSecuredSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topic, serverPublicKey, clientPublicKey, clientSecretKey, subCb, errCb);
    return subInstance;
}

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSecuredSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topics, clientPublicKey, clientSecretKey, subCb, errCb);
    return subInstance;
}


bool EZMQX::XmlSubscriber::isSecured()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::isSecured();
}

bool EZMQX::XmlSubscriber::isTerminated()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::isTerminated();
}

void EZMQX::XmlSubscriber::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::terminate();
}

std::list<EZMQX::Topic> EZMQX::XmlSubscriber::getTopics()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    return EZMQX::Subscriber::getTopics();
}