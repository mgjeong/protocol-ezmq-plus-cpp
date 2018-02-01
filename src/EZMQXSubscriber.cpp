#include <EZMQXSubscriber.h>

void EZMQX::Subscriber::initialize()
{
    return;
}

EZMQX::Subscriber::Subscriber()
{

}

EZMQX::Subscriber::Subscriber(std::string &topic, EZMQXSubCB subCb, EZMQXErrCB errCb)
{

}

EZMQX::Subscriber::Subscriber(std::list<std::string> &topics, EZMQXSubCB subCb, EZMQXErrCB errCb)
{

}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(std::string &topic, EZMQXSubCB subCb, EZMQXErrCB errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(std::list<std::string> &topics, EZMQXSubCB subCb, EZMQXErrCB errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(EZMQX::Topic &topic, EZMQXSubCB subCb, EZMQXErrCB errCb)
{
    std::shared_ptr<EZMQX::Subscriber> subInstance;
    return subInstance;
}

std::shared_ptr<EZMQX::Subscriber> EZMQX::Subscriber::getSubscriber(std::list<EZMQX::Topic> &topics, EZMQXSubCB subCb, EZMQXErrCB errCb)
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