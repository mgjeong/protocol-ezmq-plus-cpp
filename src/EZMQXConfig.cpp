#include <EZMQXConfig.h>
#include <EZMQXContext.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

EZMQX::Config::Config()
{

}

EZMQX::Config::~Config()
{

}

void EZMQX::Config::initialize()
{
    return;
}

void EZMQX::Config::terminate()
{
    return;
}

std::shared_ptr<EZMQX::Config> EZMQX::Config::getInstance()
{
    std::shared_ptr<EZMQX::Config> ptr;
    return ptr;   
}
void EZMQX::Config::addAmlModelInfo(const std::list<std::string>& amlModelInfo)
{
    ctx->addAmlRep(amlModelInfo);
    return;
}

bool EZMQX::Config::isInitialized()
{
    return false;
}

bool EZMQX::Config::isTerminated()
{
    return false;
}