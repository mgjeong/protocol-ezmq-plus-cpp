#include <EZMQXConfig.h>
#include <EZMQXContext.h>
#include <EZMQXException.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXConfig"

static const std::string LOCAL_ADDR = "localhost";

EZMQX::Config::Config(): initialized(false), ctx(EZMQX::Context::getInstance()){/*DoNotUseIt*/}

EZMQX::Config::Config(ModeOption mode): configMode(mode), initialized(false), ctx(EZMQX::Context::getInstance())
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    initialize();
}

EZMQX::Config::~Config()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

// mocking here
void EZMQX::Config::initialize()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!initialized.load())
        {
            if (StandAlone == configMode)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Set StandAlone mode", __func__);
                ctx->setStandAloneMode(true);
                ctx->setHostInfo("localhost", "localhost");
            }
            else if(Docker == configMode)
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

void EZMQX::Config::setHostInfo(std::string hostName, std::string hostAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (configMode != StandAlone)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Invalid Operation", __func__);
            throw EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
        }
        else
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Set host info Hostname: %s, Hostaddr: %s", __func__, hostName.c_str(),  hostAddr.c_str());
            ctx->setHostInfo(hostName, hostAddr);
        }
    }
}

void EZMQX::Config::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (configMode != StandAlone)
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Invalid Operation", __func__);
            throw EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
        }
        else
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Set TNS address %s", __func__, remoteAddr.c_str());
            ctx->setTnsInfo(remoteAddr);
        }
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

void EZMQX::Config::reset(ModeOption mode)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        // terminate
        EZMQX_LOG_V(DEBUG, TAG, "%s Try terminate", __func__);
        ctx->terminate();

        // clear config
        EZMQX_LOG_V(DEBUG, TAG, "%s Clear config info", __func__);
        this->configMode = mode;
        initialized.store(false);

        // initialize again
        EZMQX_LOG_V(DEBUG, TAG, "%s Initialize again", __func__);
        if (StandAlone == configMode)
        {
            ctx->setStandAloneMode(true);
            ctx->setHostInfo("localhost", "localhost");
        }
        else if(Docker == configMode)
        {
            ctx->initialize();
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Invalid Operation", __func__);
            throw EZMQX::Exception("Invalid Operation", EZMQX::UnKnownState);
        }

        initialized.store(true);
    }

    return;
}