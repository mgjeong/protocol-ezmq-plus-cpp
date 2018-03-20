#include <EZMQXContext.h>
#include <EZMQXSubscriber.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>
#include <iostream>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::Subscriber::Subscriber()
{
    // do nothing
}

EZMQX::Subscriber::Subscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false), token(""), mSubCb(subCb), mSubErrCb(errCb)
{
    verifyTopics(topics);

    try
    {
        initialize(topics, subCb, errCb);
    }
    catch(...)
    {
        throw new EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}
void EZMQX::Subscriber::internalSubCb(std::string topic, const ezmq::EZMQMessage &event)
{

    if (event.getContentType() == ezmq::EZMQ_CONTENT_TYPE_BYTEDATA && !topic.empty())
    {
        const ezmq::EZMQByteData &bytes= dynamic_cast<const ezmq::EZMQByteData&>(event);
        std::string payload((const char*)(bytes.getByteData()), bytes.getLength());
        this->que.inQue(std::make_pair(topic, payload));
    }
    else
    {
        //error handle
    }
    return;
}

EZMQX::Subscriber::Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false), token(""), mSubCb(subCb), mSubErrCb(errCb)
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
        throw new EZMQX::Exception("Could not initialize subscriber", EZMQX::UnKnownState);
    }
}

void EZMQX::Subscriber::initialize(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    // create thread and push to blocked
    mThread = std::thread(&EZMQX::Subscriber::handler, this);

    for (std::list<EZMQX::Topic>::const_iterator itr = topics.cbegin(); itr != topics.cend(); itr++)
    {
        // create subCtx with internal callback
        EZMQX::Topic topic = *itr;
        const std::string &topic_str = topic.getTopic();

        // find Aml rep
        try
        {
            std::cout << "topic: " << topic_str << " Model_Id: " << topic.getSchema() << std::endl;
            std::shared_ptr<Representation> rep = ctx->getAmlRep(topic.getSchema());

            repDic.insert(std::make_pair(topic_str, ctx->getAmlRep(topic.getSchema())));
        }
        catch(...)
        {
            throw new EZMQX::Exception("Could not found Aml Rep", EZMQX::UnKnownState);
        }

        EZMQX::Endpoint ep = topic.getEndpoint();

        std::shared_ptr<ezmq::EZMQSubscriber> sub = std::make_shared<ezmq::EZMQSubscriber>(ep.getAddr(), ep.getPort(), [](const ezmq::EZMQMessage &event)->void{ return;}, std::bind(&EZMQX::Subscriber::internalSubCb, this, std::placeholders::_1, std::placeholders::_2));
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

void EZMQX::Subscriber::handler()
{
    while(1)
    {
        std::pair<std::string, std::string> payload;

        // aml parsing here
        try
        {
            que.deQue(payload);
            if (payload.first.empty() || payload.second.empty())
            {
                throw std::runtime_error("empty payload");
                //throw new EZMQX::Exception("empty payload", EZMQX::UnKnownState);
            }

            auto itr = repDic.find(payload.first);
            std::cout << payload.first << std::endl;
            if (itr == repDic.end())
            {
                throw std::runtime_error("Could not find Aml rep");
                //throw new EZMQX::Exception("Could not find Aml rep", EZMQX::UnKnownState);
            }
            else
            {
                AMLObject *obj = itr->second->ByteToData(payload.second);

                if (!obj)
                {
                    throw std::runtime_error("Could not convert byte to AMLObject");
                    //throw new EZMQX::Exception("Could not convert byte to AMLObject", EZMQX::UnKnownState);
                }

                // call subCb
                mSubCb(payload.first, *obj);

            }
        }
        catch(const std::exception &e)
        {
            // call errCb
            std::cout << e.what() << std::endl;
            mSubErrCb(payload.first, payload.first.empty() ? UnknownTopic : BrokenPayload);
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