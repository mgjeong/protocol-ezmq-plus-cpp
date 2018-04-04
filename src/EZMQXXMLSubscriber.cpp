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

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::XmlSubscriber::XmlSubscriber()
{
    // do nothing
}

EZMQX::XmlSubscriber::~XmlSubscriber()
{
    // do nothing
}

EZMQX::XmlSubscriber::XmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

EZMQX::XmlSubscriber::XmlSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : Subscriber(), mSubCb(subCb), mSubErrCb(errCb)
{
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


void EZMQX::XmlSubscriber::cb(const std::string &_topic, const AMLObject *obj)
{
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

std::shared_ptr<EZMQX::XmlSubscriber> EZMQX::XmlSubscriber::getSubscriber(const std::string &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::XmlSubscriber> subInstance(new XmlSubscriber(topic, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::XmlSubscriber> EZMQX::XmlSubscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::list<EZMQX::Topic> topics(1, topic);
    std::shared_ptr<EZMQX::XmlSubscriber> subInstance(new XmlSubscriber(topics, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::XmlSubscriber> EZMQX::XmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::XmlSubscriber> subInstance(new XmlSubscriber(topics, subCb, errCb));
    return subInstance;
}

bool EZMQX::XmlSubscriber::isTerminated()
{
    return EZMQX::Subscriber::isTerminated();
}

void EZMQX::XmlSubscriber::terminate()
{
    return EZMQX::Subscriber::terminate();
}

std::list<EZMQX::Topic> EZMQX::XmlSubscriber::getTopics()
{
    return EZMQX::Subscriber::getTopics();
}