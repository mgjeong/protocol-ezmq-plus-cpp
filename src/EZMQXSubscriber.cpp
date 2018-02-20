#include <EZMQXContext.h>
#include <EZMQXSubscriber.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::Subscriber::Subscriber()
{
    // do nothing
}

EZMQX::Subscriber::Subscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false)
{

}

EZMQX::Subscriber::Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
 : terminated(false)
{

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