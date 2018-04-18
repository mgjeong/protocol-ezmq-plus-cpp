#include <EZMQXFakeSingletonAccessor.h>
#include <EZMQXContext.h>

static const std::string FAKE_HOST_NAME = "localhost";
static const std::string FAKE_HOST_ADDR = "localhost";
static const std::string FAKE_REMOTE_ADDR = "localhost";
static const int LOCAL_PORT_START = 4000;
static const int LOCAL_PORT_MAX = 100;

void EZMQX::FakeSingletonAccessor::setFake()
{
    EZMQX::Context* ctx = EZMQX::Context::getInstance();
    ctx->setHostInfo(FAKE_HOST_NAME, FAKE_HOST_ADDR);
    ctx->setTnsInfo(FAKE_REMOTE_ADDR);

    for (int i = LOCAL_PORT_START; i <= LOCAL_PORT_MAX; i++)
    {
        // set dummy
        ctx->ports[i] = i;
    }

    ctx->initialized.store(true);

    return;
}