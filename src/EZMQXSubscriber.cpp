#include <EZMQXContext.h>
#include <EZMQXSubscriber.h>


static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::Subscriber::Subscriber()
{

}

EZMQX::Subscriber::Subscriber(const  std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{

}

EZMQX::Subscriber::Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{

}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const EZMQX::Topic &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

bool EZMQX::Subscriber::isTerminated()
{
    return false;
}

void EZMQX::Subscriber::terminate()
{
    return;
}