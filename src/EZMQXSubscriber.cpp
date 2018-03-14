#include <EZMQXContext.h>
#include <EZMQXSubscriber.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQErrorCodes.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
static ezmq::EZMQSubCB ezmqSubCb = [](const ezmq::EZMQMessage &event)->void{ return;};
static ezmq::EZMQSubTopicCB ezmqSubTopicCb = [](std::string topic, const ezmq::EZMQMessage &event)->void{ return;};

EZMQX::Subscriber::Subscriber()
{
    // do nothing
}

EZMQX::Subscriber::Subscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false), token("")
{
    verifyTopics(topics);

    try
    {
        initialize(topics, subCb, errCb);
    }
    catch(...)
    {
        // throw exception
    }
}

EZMQX::Subscriber::Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false), token("")
{
    std::list<EZMQX::Topic> verified;
    verifyTopics(topics, verified);

    if (verified.empty())
    {
        // throw exception
    }

    try
    {
        initialize(verified, subCb, errCb);
    }
    catch(...)
    {
        // throw exception
    }
}

void EZMQX::Subscriber::initialize(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getTopic();
        EZMQX::Endpoint ep = topic.getEndpoint();
        std::shared_ptr<ezmq::EZMQSubscriber> sub = std::make_shared<ezmq::EZMQSubscriber>(ep.getAddr(), ep.getPort(), ezmqSubCb, ezmqSubTopicCb);
        subscribers.push_back(sub);

        // store pair of topics and callbacks on map
        callbacks.insert(std::pair<std::string, std::pair<SubCb, SubErrCb>>(topic_str, std::make_pair(subCb, errCb)));

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

void EZMQX::Subscriber::verifyTopics(const std::list<std::string> &topics, std::list<EZMQX::Topic> &verified)
{
    return;
}

// throw exception when topic is not mached
void EZMQX::Subscriber::verifyTopics(const std::list<EZMQX::Topic> &topics)
{
    return;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::list<std::string> topics(1, topic);
    std::shared_ptr<EZMQX::Subscriber> subInstance(new Subscriber(topics, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance(new Subscriber(topics, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::list<EZMQX::Topic> topics(1, topic);
    std::shared_ptr<EZMQX::Subscriber> subInstance(new Subscriber(topics, subCb, errCb));
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance(new Subscriber(topics, subCb, errCb));
    return subInstance;
}

bool EZMQX::Subscriber::isTerminated()
{
    // atomically
    return terminated.load();
}

void EZMQX::Subscriber::terminate()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // release resource
        }
        else
        {
            throw EZMQX::Exception("Subscriber terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    return;
}

std::list<EZMQX::Topic> EZMQX::Subscriber::getTopics()
{
    std::list<EZMQX::Topic> topics;
    return topics;
}