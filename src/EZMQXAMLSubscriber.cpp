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

#include <EZMQXAMLSubscriber.h>
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

#define TAG "EZMQXAmlSubscriber"
#define KEY_LENGTH 40

EZMQX::AmlSubscriber::~AmlSubscriber()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

EZMQX::AmlSubscriber::AmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

EZMQX::AmlSubscriber::AmlSubscriber(const std::string &topic, bool isHierarchical, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

EZMQX::AmlSubscriber::AmlSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

EZMQX::AmlSubscriber::AmlSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

void EZMQX::AmlSubscriber::cb(const std::string &_topic, const AML::AMLObject *obj)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (ctx->isTerminated())
    {
        terminate();
        throw EZMQX::Exception("Subscriber terminated", EZMQX::Terminated);
    }

    if (!_topic.empty() && obj != NULL)
    {
        // call subCb
        mSubCb(_topic, *obj);
    }
    else
    {
        // call errCb
        mSubErrCb(_topic, _topic.empty() ? UnknownTopic : BrokenPayload);
    }

    return;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const std::string &topic, bool isHierarchical, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topic, isHierarchical, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (topic.isSecured())
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s topic is secured!!!", __func__);
        throw EZMQX::Exception("topic is secured", EZMQX::InvalidParam);
    }

    std::list<EZMQX::Topic> topics(1, topic);
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        if ((*itr).isSecured())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s topic is secured!!!", __func__);
            throw EZMQX::Exception("topic is secured", EZMQX::InvalidParam);
        }
    }

    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSecuredSubscriber(const EZMQX::Topic &topic, const std::string &serverPublicKey, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (!topic.isSecured())
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s topic is unsecured!!!", __func__);
        throw EZMQX::Exception("topic is unsecured", EZMQX::InvalidParam);
    }

    if (serverPublicKey.length() != KEY_LENGTH || clientPublicKey.length() != KEY_LENGTH || clientSecretKey.length() != KEY_LENGTH)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid key length!!!", __func__);
        throw EZMQX::Exception("Invalid key length", EZMQX::InvalidParam);
    }

    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topic, serverPublicKey, clientPublicKey, clientSecretKey, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSecuredSubscriber(const std::map<EZMQX::Topic, std::string> &topics, const std::string &clientPublicKey, const std::string &clientSecretKey, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    for (std::map<EZMQX::Topic, std::string>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        if ((*itr).first.isSecured() == false)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s topic is unsecured!!!", __func__);
            throw EZMQX::Exception("topic is unsecured", EZMQX::InvalidParam);
        }

        if ((*itr).second.length() != KEY_LENGTH)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Invalid key length!!!", __func__);
            throw EZMQX::Exception("Invalid key length", EZMQX::InvalidParam);
        }
    }

    if (clientPublicKey.length() != KEY_LENGTH || clientSecretKey.length() != KEY_LENGTH)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Invalid key length!!!", __func__);
        throw EZMQX::Exception("Invalid key length", EZMQX::InvalidParam);
    }

    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, clientPublicKey, clientSecretKey, subCb, errCb);
    return subInstance;
}

bool EZMQX::AmlSubscriber::isSecured()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::isSecured();
}

bool EZMQX::AmlSubscriber::isTerminated()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::isTerminated();
}

void EZMQX::AmlSubscriber::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::terminate();
}

std::list<EZMQX::Topic> EZMQX::AmlSubscriber::getTopics()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return EZMQX::Subscriber::getTopics();
}