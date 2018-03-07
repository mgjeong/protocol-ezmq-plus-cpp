#include <EZMQXFakeSingletonAccessor.h>
#include <EZMQXContext.h>

static const std::string FAKE_HOST_NAME = "localhost";
static const std::string FAKE_HOST_ADDR = "localhost";
static const std::string FAKE_REMOTE_ADDR = "localhost";
static const int FAKE_RESERVE_PORT = 4000;

void EZMQX::FakeSingletonAccessor::setFake()
{
    try
    {
        std::map<int, int> fakePorts;
        for (int i = 0; i < 100; i++)
        {
            fakePorts.insert(std::pair<int, int>(FAKE_RESERVE_PORT+i, FAKE_RESERVE_PORT+i));
        }

        EZMQX::Context::_instance.reset(new EZMQX::Context(FAKE_HOST_NAME, FAKE_HOST_ADDR, FAKE_REMOTE_ADDR, fakePorts));
    }
    catch(...)
    {
        // do nothing
    }

    
    return;
}