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

EZMQX::XmlSubscriber::XmlSubscriber() : Subscriber(), mSubCb([](std::string topic, const std::string& payload){}), mSubErrCb([](std::string topic, EZMQX::ErrorCode errCode){})
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // do nothing
}

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

EZMQX::XmlSubscriber::XmlSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    try
    {
        Subscriber::initialize(topic);
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

EZMQX::XmlSubscriber* EZMQX::XmlSubscriber::getSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX::XmlSubscriber* subInstance = new XmlSubscriber(topic, subCb, errCb);
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