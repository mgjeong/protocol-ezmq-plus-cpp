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

EZMQX::AmlSubscriber::AmlSubscriber() : Subscriber(), mSubCb([](std::string topic, const AML::AMLObject& payload){}), mSubErrCb([](std::string topic, EZMQX::ErrorCode errCode){})
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // do nothing
}

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

void EZMQX::AmlSubscriber::cb(const std::string &_topic, const AML::AMLObject *obj)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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
    std::list<EZMQX::Topic> topics(1, topic);
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
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