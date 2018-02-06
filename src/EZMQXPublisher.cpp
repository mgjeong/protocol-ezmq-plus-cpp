#include <EZMQXContext.h>
#include <EZMQXPublisher.h>
#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::Publisher::Publisher(const std::string &topic, const std::string &schema, const EZMQX::PubErrCb &errCb)
 : terminated(false)
{
    //validate topic

    //get Random port
    int port;

    //create EZMQ publisher
        // loop for validate port

    //get Host Ep
    EZMQX::Topic _topic(topic, schema, ctx->getHostEp(port));

    //register topic
    //register here
    this->topic = _topic;
}

std::shared_ptr<EZMQX::Publisher> EZMQX::Publisher::getPublisher(const std::string &topic, const std::string &schema, const EZMQX::PubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Publisher> pubInstance(new Publisher(topic, schema, errCb));
    return pubInstance;
}

void EZMQX::Publisher::publish(void* object)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (!terminated.load())
        {

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
    //atomically
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
        }
        else
        {
            throw new EZMQX::Exception("Publisher terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    return;
}