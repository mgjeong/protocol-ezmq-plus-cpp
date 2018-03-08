#include <EZMQXContext.h>
#include <EZMQXPublisher.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <Representation.h>
#include <EZMQErrorCodes.h>
#include <AMLException.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
static std::function<void(ezmq::EZMQErrorCode code)> ezmqCb = [](ezmq::EZMQErrorCode code)->void{return;};
//static ezmq::EZMQStartCB startCb = [](ezmq::EZMQErrorCode){};
//static ezmq::EZMQStopCB stopCb = [](ezmq::EZMQErrorCode){};
//static ezmq::EZMQErrorCB errorCb = [](ezmq::EZMQErrorCode){};

EZMQX::Publisher::Publisher(const std::string &topic, const std::string &amlModelFilePath, const EZMQX::PubErrCb &errCb)
 : terminated(false), localPort(0)
{
    //validate topic

    localPort = ctx->assignDynamicPort();
    // create ezmq publisher
    // ezmq error callback should provide shared pointer in callback
    pubCtx = std::make_shared<ezmq::EZMQPublisher>(localPort, ezmqCb, ezmqCb, ezmqCb);

    //get Aml Model Id
    std::string modelId="";

    //get Host Ep
    EZMQX::Topic _topic(topic, modelId, ctx->getHostEp(localPort));

    //register topic //throw exception
    registerTopic(_topic);


    //register here
    this->topic = _topic;
}

EZMQX::Publisher::~Publisher()
{
    terminate();
}

std::shared_ptr<EZMQX::Publisher> EZMQX::Publisher::getPublisher(const std::string &topic, const std::string &amlModelFilePath, const EZMQX::PubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Publisher> pubInstance(new Publisher(topic, amlModelFilePath, errCb));
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
            std::shared_ptr<Representation> rep = ctx->getAmlRep(topic.getSchema());
            // transform // throw exception
            std::string byteAml = rep->DataToByte(payload);

            // build payload

            // publish
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