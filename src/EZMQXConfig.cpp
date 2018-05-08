#include <EZMQXConfig.h>
#include <EZMQXContext.h>
#include <EZMQXException.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXConfig"

static const std::string LOCAL_ADDR = "localhost";

EZMQX::Config::Config(): initialized(false), ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

EZMQX::Config::~Config()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Config::startDockerMode()
{
    initialize(EZMQX::Docker);
    return;
}

void EZMQX::Config::startStandAloneMode(bool useTns, std::string tnsAddr)
{
    initialize(EZMQX::StandAlone);

    if (useTns)
    {
        setTnsInfo(tnsAddr);
    }

    return;
}

void EZMQX::Config::initialize(EZMQX::ModeOption configMode)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (initialized.load())
        {
            throw EZMQX::Exception("Could not initialize already initialized", EZMQX::Initialized);
        }
        else
        {
            if (EZMQX::StandAlone == configMode)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Set StandAlone mode", __func__);
                ctx->setStandAloneMode(true);
                ctx->setHostInfo("localhost", "localhost");
            }
            else if(EZMQX::Docker == configMode)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Set as Docker", __func__);
                ctx->initialize();
            }
            else
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Invalid Operation", __func__);
                throw EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
            }

            initialized.store(true);
        }
    }
    // mutex unlock

    return;
}

void EZMQX::Config::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        EZMQX_LOG_V(DEBUG, TAG, "%s Set TNS address %s", __func__, remoteAddr.c_str());
        ctx->setTnsInfo(remoteAddr);
    }
    // mutex unlock
}

// mocking here
void EZMQX::Config::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        ctx->terminate();
    }
    // mutex unlock

    return;
}

std::list<std::string> EZMQX::Config::addAmlModel(const std::list<std::string>& amlFilePath)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return ctx->addAmlRep(amlFilePath);
}

void EZMQX::Config::reset()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        // terminate
        EZMQX_LOG_V(DEBUG, TAG, "%s Try terminate", __func__);
        ctx->terminate();

        // clear config
        EZMQX_LOG_V(DEBUG, TAG, "%s Clear config info", __func__);
        initialized.store(false);
    }

    return;
}