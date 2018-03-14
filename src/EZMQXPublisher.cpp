#include <EZMQXContext.h>
#include <EZMQXPublisher.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <Representation.h>
#include <AMLException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
static std::function<void(ezmq::EZMQErrorCode code)> ezmqCb = [](ezmq::EZMQErrorCode code)->void{std::cout<<"errCb"<<std::endl; return;};

EZMQX::Publisher::Publisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo)
 : terminated(false), localPort(0)
{
    //validate topic
    try
    {
        localPort = ctx->assignDynamicPort();
    }
    catch(...)
    {
        // throw maximum port exceed exception
    }

    // create ezmq publisher
    // ezmq error callback should provide shared pointer in callback
    pubCtx = std::make_shared<ezmq::EZMQPublisher>(localPort, ezmqCb, ezmqCb, ezmqCb);

    //get Aml Model Id
    std::string modelId="";
    if (infoType == AmlModelId)
    {
        try
        {
            rep = ctx->getAmlRep(amlModelInfo);
        }
        catch(...)
        {
            //throw model not exist exception
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
        catch(...)
        {
            //throw AML model parse error
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
        _topic = EZMQX::Topic(topic, modelId, ctx->getHostEp(localPort));
    }
    catch(...)
    {
        throw new EZMQX::Exception("Invalid Port", EZMQX::UnKnownState);
    }

    //register topic //throw exception
    registerTopic(_topic);


    //register here
    this->topic = _topic;
}

EZMQX::Publisher::~Publisher()
{
    terminate();
}

std::shared_ptr<EZMQX::Publisher> EZMQX::Publisher::getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo)
{
    std::shared_ptr<EZMQX::Publisher> pubInstance(new Publisher(topic, infoType, amlModelInfo));
    return pubInstance;
}

void EZMQX::Publisher::registerTopic(const EZMQX::Topic& topic)
{
    return;
}

void EZMQX::Publisher::publish(const AMLObject& payload)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // get AML model id
            // get AMLRep
            if (!rep)
            {
                rep = ctx->getAmlRep(topic.getSchema());
            }

            // transform // throw exception
            std::string byteAml = rep->DataToByte(payload);

            // publish
            if (!pubCtx)
            {
                // throw exception
            }

            ezmq::EZMQByteData data(reinterpret_cast<const uint8_t*>(byteAml.c_str()), byteAml.length());
            pubCtx->publish(topic.getTopic(), data);
        }
        else
        {
            throw new EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }
    }
    // mutex unlock
    return;
}

bool EZMQX::Publisher::isTerminated()
{
    // atomically
    return terminated.load();
}

void EZMQX::Publisher::terminate()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {
            // release resource
            ctx->releaseDynamicPort(localPort);
        }
        else
        {
            throw EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    return;
}