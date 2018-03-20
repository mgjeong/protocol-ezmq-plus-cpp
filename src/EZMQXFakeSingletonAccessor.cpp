#include <EZMQXFakeSingletonAccessor.h>
#include <EZMQXContext.h>

static const std::string FAKE_HOST_NAME = "localhost";
static const std::string FAKE_HOST_ADDR = "localhost";
static const std::string FAKE_REMOTE_ADDR = "localhost";

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();

void EZMQX::FakeSingletonAccessor::setFake()
{
    try
    {
        ctx->setStandAloneMode(true);
        ctx->setHostInfo(FAKE_HOST_NAME, FAKE_HOST_ADDR);
        ctx->setTnsInfo(FAKE_REMOTE_ADDR);
    }
    catch(...)
    {
        // do nothing
    }

    
    return;
}