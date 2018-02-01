#include <EZMQXPublisher.h>
#include <EZMQXContext.h>

EZMQX::Publisher::Publisher(std::string &topic) : ctx(EZMQX::Context::getInstance())
{
    
}

std::shared_ptr<EZMQX::Publisher> EZMQX::Publisher::getPublisher(std::string &topic)
{
    std::shared_ptr<EZMQX::Publisher> pubInstance(new Publisher(topic));
    return pubInstance;
}

void EZMQX::Publisher::initialize()
{
    return;
}

void EZMQX::Publisher::publish(void* object)
{
    return;
}

bool EZMQX::Publisher::isTerminated()
{
    return false;
}

void EZMQX::Publisher::terminate()
{
    return;
}