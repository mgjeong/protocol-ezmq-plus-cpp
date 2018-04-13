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

EZMQX::AmlSubscriber::AmlSubscriber()
{
    // do nothing
}

EZMQX::AmlSubscriber::~AmlSubscriber()
{
    // do nothing
}

EZMQX::AmlSubscriber::AmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    if (ctx->isTnsEnabled())
    {
        verifyTopics(topics);
    }

    try
    {
        Subscriber::initialize(topics);
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

EZMQX::AmlSubscriber::AmlSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
    validateTopic(topic);

    std::list<EZMQX::Topic> verified;

    if (ctx->isTnsEnabled())
    {
        verifyTopics(topic, verified);

        if (verified.empty())
        {
            // throw exception
        }
    }

    try
    {
        Subscriber::initialize(verified);
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

void EZMQX::AmlSubscriber::cb(const std::string &_topic, const AML::AMLObject *obj)
{
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

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topic, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::list<EZMQX::Topic> topics(1, topic);
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

EZMQX::AmlSubscriber* EZMQX::AmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    EZMQX::AmlSubscriber* subInstance = new AmlSubscriber(topics, subCb, errCb);
    return subInstance;
}

bool EZMQX::AmlSubscriber::isTerminated()
{
    return EZMQX::Subscriber::isTerminated();
}

void EZMQX::AmlSubscriber::terminate()
{
    return EZMQX::Subscriber::terminate();
}

std::list<EZMQX::Topic> EZMQX::AmlSubscriber::getTopics()
{
    return EZMQX::Subscriber::getTopics();
}