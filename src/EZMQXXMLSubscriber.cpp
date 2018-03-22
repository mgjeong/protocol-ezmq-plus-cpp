#include <EZMQXContext.h>
#include <EZMQXXMLSubscriber.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>
#include <EZMQXRest.h>
#include <iostream>
#include <json/reader.h>
#include <EZMQXBlockingQue.h>

static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";

static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_SCHEMA = "schema";
static const std::string QUERY_PARAM = "topic=";

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::XmlSubscriber::XmlSubscriber()
{
    // do nothing
}

EZMQX::XmlSubscriber::XmlSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::XmlSubCb &subCb, EZMQX::SubErrCb &errCb)
 : que(new EZMQX::BlockingQue()), terminated(false), token(""), mSubCb(subCb), mSubErrCb(errCb)
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
 : que(new EZMQX::BlockingQue()), terminated(false), token(""), mSubCb(subCb), mSubErrCb(errCb)
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

void EZMQX::XmlSubscriber::internalSubCb(std::string topic, const ezmq::EZMQMessage &event)
{

    if (event.getContentType() == ezmq::EZMQ_CONTENT_TYPE_BYTEDATA && !topic.empty())
    {
        const ezmq::EZMQByteData &bytes= dynamic_cast<const ezmq::EZMQByteData&>(event);
        std::string payload((const char*)(bytes.getByteData()), bytes.getLength());
        this->que->inQue(std::make_pair(topic, payload));
    }
    else
    {
        //error handle
    }
    return;
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

void EZMQX::XmlSubscriber::handler()
{
    while(1)
    {
        std::pair<std::string, std::string> payload;

        // aml parsing here
        try
        {
            que->deQue(payload);
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
                std::shared_ptr<Representation> rep = itr->second;
                AMLObject *obj = rep->ByteToData(payload.second);
                if (!obj)
                {
                    throw std::runtime_error("Could not convert byte to AMLObject");
                    //throw new EZMQX::Exception("Could not convert byte to AMLObject", EZMQX::UnKnownState);
                }

                std::string xml = rep->DataToAml(*obj);
                if (xml.empty())
                {
                    throw std::runtime_error("Could not convert AMLObject to Xml");
                }

                // call subCb
                mSubCb(payload.first, xml);

                // TODO: recursive dtor not provided yet
                // delete obj;

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

void EZMQX::XmlSubscriber::verifyTopics(const std::string &topic, std::list<EZMQX::Topic> &verified)
{   
    std::string tmp;
    // send rest
    try
    {
        EZMQX::SimpleRest rest;
        tmp = rest.Get(ctx->getTnsAddr() + PREFIX + TOPIC, QUERY_PARAM + topic);
    }
    catch (...)
    {
        throw EZMQX::Exception("Could not send rest get request", EZMQX::UnKnownState);
    }

    // parse json
    try
    {
        Json::Value props;
        Json::Reader reader;

        // try parse json
        if((!reader.parse(tmp, props)) && props.type() != Json::arrayValue)
        {
            throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
        }
        else
        {
            // access array
            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
            {
                // get Topic
                if (props[i].isMember(PAYLOAD_TOPIC) && props[i].isMember(PAYLOAD_SCHEMA) && props[i].isMember(PAYLOAD_ENDPOINT))
                {
                    verified.push_back(EZMQX::Topic(props[i][PAYLOAD_TOPIC].asString(), props[i][PAYLOAD_SCHEMA].asString(), EZMQX::Endpoint(props[i][PAYLOAD_ENDPOINT].asString())));
                }
            }

        }
    }
    catch(...)
    {
        throw EZMQX::Exception("Could not parse json object", EZMQX::UnKnownState);
    }

    // made list


    return;
}

// throw exception when topic is not mached
void EZMQX::XmlSubscriber::verifyTopics(const std::list<EZMQX::Topic> &topics)
{
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
    // atomically
    return terminated.load();
}

void EZMQX::XmlSubscriber::terminate()
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

std::list<EZMQX::Topic> EZMQX::XmlSubscriber::getTopics()
{
    std::list<EZMQX::Topic> topics;
    return topics;
}