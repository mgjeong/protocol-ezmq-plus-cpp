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

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

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
        initialize(topics, subCb, errCb);
    }
    catch(...)
    {
        throw new EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

EZMQX::AmlSubscriber::AmlSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
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

void EZMQX::AmlSubscriber::initialize(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    // create thread and push to blocked
    mThread = std::thread(&EZMQX::AmlSubscriber::handler, this);

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

        std::shared_ptr<ezmq::EZMQSubscriber> sub = std::make_shared<ezmq::EZMQSubscriber>(ep.getAddr(), ep.getPort(), [](const ezmq::EZMQMessage &event)->void{ return;}, std::bind(&EZMQX::AmlSubscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));
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

void EZMQX::AmlSubscriber::cb(const std::string &_topic, const AMLObject *obj)
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

std::shared_ptr<EZMQX::AmlSubscriber> EZMQX::AmlSubscriber::getSubscriber(const std::string &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::AmlSubscriber> subInstance(new AmlSubscriber(topic, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::AmlSubscriber> EZMQX::AmlSubscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::list<EZMQX::Topic> topics(1, topic);
    std::shared_ptr<EZMQX::AmlSubscriber> subInstance(new AmlSubscriber(topics, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::AmlSubscriber> EZMQX::AmlSubscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::AmlSubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::AmlSubscriber> subInstance(new AmlSubscriber(topics, subCb, errCb));
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