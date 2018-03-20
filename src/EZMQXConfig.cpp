#include <EZMQXConfig.h>
#include <EZMQXContext.h>
#include <EZMQXException.h>

static const std::string LOCAL_ADDR = "localhost";
static std::shared_ptr<EZMQX::Context> ctx = EZMQX::Context::getInstance();
std::shared_ptr<EZMQX::Config> EZMQX::Config::_instance;

EZMQX::Config::Config(): initialized(false){}

EZMQX::Config::Config(ModeOption mode): configMode(mode), initialized(false)
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
            if (StandAlone == configMode)
            {
                ctx->setStandAloneMode(true);
                ctx->setHostInfo("localhost", "localhost");
            }
            else if(FullFeature == configMode)
            {
                ctx->initialize();
            }
            else
            {
                throw new EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
            }

            initialized.store(true);
        }
    }
    // mutex unlock

    return;
}

void EZMQX::Config::setHostInfo(std::string hostName, std::string hostAddr)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (configMode != StandAlone)
        {
            throw new EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
        }
        else
        {
            ctx->setHostInfo(hostName, hostAddr);
        }
    }
}

void EZMQX::Config::setTnsInfo(std::string remoteAddr)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (configMode != StandAlone)
        {
            throw new EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
        }
        else
        {
            ctx->setTnsInfo(remoteAddr);
        }
    }
    // mutex unlock
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

std::shared_ptr<EZMQX::Config> EZMQX::Config::getInstance(ModeOption mode)
{
    if (!_instance)
    {
        _instance.reset(new EZMQX::Config(mode));
    }

    return _instance;
}

std::list<std::string> EZMQX::Config::addAmlModel(const std::list<std::string>& amlFilePath)
{
    return ctx->addAmlRep(amlFilePath);
}