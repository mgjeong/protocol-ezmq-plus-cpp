#include <EZMQXConfig.h>
#include <EZMQXContext.h>

static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
std::shared_ptr<EZMQX::Config> EZMQX::Config::_instance;

EZMQX::Config::Config()
{
    initialize();
}

EZMQX::Config::~Config()
{
    terminate();
}

// mocking here
void EZMQX::Config::initialize()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!initialized.load())
        {

            initialized.store(true);
        }
    }
    // mutex unlock

    return;
}

// mocking here
void EZMQX::Config::terminate()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
    }
    // mutex unlock

    return;
}

std::shared_ptr<EZMQX::Config> EZMQX::Config::getInstance()
{
    if (!_instance)
    {
        _instance.reset(new EZMQX::Config());
    }

    return _instance;
}

std::list<std::string> EZMQX::Config::addAmlModel(const std::list<std::string>& amlFilePath)
{
    return ctx->addAmlRep(amlFilePath);
}