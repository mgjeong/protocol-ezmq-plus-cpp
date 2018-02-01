#include <EZMQXContext.h>

std::shared_ptr<EZMQX::Context> EZMQX::Context::_instance;

// ctor
EZMQX::Context::Context() : initialized(false)
{
    initialize();
}

// use mutex
void EZMQX::Context::initialize()
{
    return;
}

std::shared_ptr<EZMQX::Context> EZMQX::Context::getInstance()
{
    if (!_instance)
    {
        _instance = std::shared_ptr<EZMQX::Context>(new EZMQX::Context());
    }
    _instance.reset(new EZMQX::Context);

    return _instance;
}

bool EZMQX::Context::isInitialized()
{
    return false;
}

EZMQX::Endpoint EZMQX::Context::getHostEp(int port)
{
    EZMQX::Endpoint ep("", -1);
    return ep;
}

bool EZMQX::Context::isTerminated()
{
    return false;
}

void EZMQX::Context::terminate()
{
    return;
}