#include <EZMQXAmlPublisher.h>
#include <EZMQXException.h>
#include <EZMQXContext.h>
#include <Representation.h>
#include <AMLException.h>
#include <EZMQErrorCodes.h>
#include <EZMQByteData.h>

EZMQX::AmlPublisher::AmlPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort)
 : Publisher(optionalPort)
{
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
        _topic = EZMQX::Topic(topic, rep->getRepresentationId(), ctx->getHostEp(localPort));
    }
    catch(...)
    {
        throw EZMQX::Exception("Invalid Port", EZMQX::UnKnownState);
    }

    registerTopic(_topic);
}

EZMQX::AmlPublisher::~AmlPublisher()
{

}

EZMQX::AmlPublisher* EZMQX::AmlPublisher::getPublisher(const std::string &topic, const EZMQX::AmlModelInfo& infoType, const std::string &amlModelInfo, int optionalPort)
{
    EZMQX::AmlPublisher* pubInstance = new AmlPublisher(topic, infoType, amlModelInfo, optionalPort);
    return pubInstance;
}

void EZMQX::AmlPublisher::publish(const AMLObject& payload)
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
            throw EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }
    }
    // mutex unlock
    return;
}