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
        initialize(topics, subCb, errCb);
    }
    catch(...)
    {
        throw new EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
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
        initialize(verified, subCb, errCb);
    }
    catch(...)
    {
        throw new EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

void EZMQX::XmlSubscriber::initialize(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    // create thread and push to blocked
    mThread = std::thread(&EZMQX::XmlSubscriber::handler, this);

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getTopic();

        // find Aml rep
        try
        {
            std::cout << "Topic: " << topic_str << " Model_Id: " << topic.getSchema() << " Endpoint: " << topic.getEndpoint().toString() << std::endl;
            std::shared_ptr<Representation> rep = ctx->getAmlRep(topic.getSchema());

            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getSchema())));
        }
        catch(...)
        {
            throw new EZMQX::Exception("Could not found Aml Rep", EZMQX::UnKnownState);
        }

        EZMQX::Endpoint ep = topic.getEndpoint();

        std::shared_ptr<ezmq::EZMQSubscriber> sub = std::make_shared<ezmq::EZMQSubscriber>(ep.getAddr(), ep.getPort(), [](const ezmq::EZMQMessage &event)->void{ return;}, std::bind(&EZMQX::XmlSubscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));
        subscribers.push_back(sub);

        ezmq::EZMQErrorCode ret = sub->start();

        if (ezmq::EZMQ_OK != ret)
        {
            // throw exception
        }

        ret = sub->subscribe(topic_str);

        if (ezmq::EZMQ_OK != ret)
        {
            // throw exception
        }
    }

    return;
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