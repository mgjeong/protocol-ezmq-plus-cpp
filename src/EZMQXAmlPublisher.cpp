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

#include <EZMQXAmlPublisher.h>
#include <EZMQXException.h>
#include <EZMQXContext.h>
#include <Representation.h>
#include <AMLException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>
#include <EZMQXException.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXAmlPublisher"

EZMQX::AmlPublisher::AmlPublisher(const std::string &topic, const std::string &privateKey, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort)
 : Publisher(optionalPort, privateKey)
{
    validateTopic(topic);

    if (infoType == AmlModelId)
    {
        try
        {
            rep = ctx->getAmlRep(amlModelInfo);
        }
        catch (const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            //throw model not exist exception
            throw EZMQX::Exception("Could not find given AML model id", EZMQX::UnknownAmlModel);
        }
    }
    else if (infoType == AmlFilePath)
    {
        try
        {
            std::list<std::string> tmp(1, amlModelInfo);
            tmp = ctx->addAmlRep(tmp);
            rep = ctx->getAmlRep(*(tmp.begin()));
        }
        catch (const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            //throw AML model parse error
            throw EZMQX::Exception("Could not parse given AML model file", EZMQX::InvalidAmlModel);
        }
    }
    else
    {
        // throw wrong option exception
    }

    //get Host Ep
    EZMQX::Topic _topic;
    try
    {
        _topic = EZMQX::Topic(topic, rep->getRepresentationId(), secured, ctx->getHostEp(localPort));
    }
    catch (const EZMQX::Exception& e)
    {
        throw e;
    }
    catch(...)
    {
        throw EZMQX::Exception("Unnknown state", EZMQX::UnKnownState);
    }

    registerTopic(_topic);
}

EZMQX::AmlPublisher::~AmlPublisher()
{

}

EZMQX::AmlPublisher* EZMQX::AmlPublisher::getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort)
{
    std::string emptyString = "";
    EZMQX::AmlPublisher* pubInstance = new AmlPublisher(topic, emptyString, infoType, amlModelInfo, optionalPort);
    return pubInstance;
}

EZMQX::AmlPublisher* EZMQX::AmlPublisher::getSecuredPublisher(const std::string &topic, const std::string &privateKey, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort)
{
    EZMQX::AmlPublisher* pubInstance = new AmlPublisher(topic, privateKey, infoType, amlModelInfo, optionalPort);
    return pubInstance;
}

void EZMQX::AmlPublisher::publish(const AML::AMLObject& payload)
{
    if (ctx->isTerminated())
    {
        terminate();
        throw EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
    }

    // get AML model id
    // get AMLRep
    if (!rep)
    {
        rep = ctx->getAmlRep(topic.getDatamodel());
    }

    // transform // throw exception
    std::string byteAml = rep->DataToByte(payload);

    // publish
    if (!pubCtx)
    {
        // throw exception
        EZMQX_LOG_V(ERROR, TAG, "%s Could publish payload, pubCtx is null", __func__);
        throw EZMQX::Exception("Could publish payload, pubCtx is null", EZMQX::UnKnownState);
    }

    ezmq::EZMQByteData data(reinterpret_cast<const uint8_t*>(byteAml.c_str()), byteAml.length());
    pubCtx->publish(topic.getName(), data);
    return;
}

EZMQX::Topic EZMQX::AmlPublisher::getTopic()
{
    return Publisher::getTopic();
}

bool EZMQX::AmlPublisher::isSecured()
{
    return Publisher::isSecured();
}

bool EZMQX::AmlPublisher::isTerminated()
{
    return EZMQX::Publisher::isTerminated();
}

void EZMQX::AmlPublisher::terminate()
{
    return EZMQX::Publisher::terminate();
}