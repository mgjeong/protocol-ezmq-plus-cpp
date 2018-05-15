#include <EZMQXConfig.h>
#include <EZMQXContext.h>
#include <EZMQXException.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXConfig"

static const std::string LOCAL_ADDR = "localhost";

EZMQX::Config* EZMQX::Config::getInstance()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    static EZMQX::Config _instance;
    return &_instance;
}

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
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        try
        {
            std::lock_guard<std::mutex> scopedLock(lock);
            initialize(EZMQX::Docker);
        }
        catch(const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not start ezmq plus", __func__);
            throw EZMQX::Exception("Could not start ezmq plus", EZMQX::UnKnownState);
        }
    }
    // mutex unlock

    return;
}

void EZMQX::Config::startStandAloneMode(bool useTns, std::string tnsAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        try
        {
            std::lock_guard<std::mutex> scopedLock(lock);
            initialize(EZMQX::StandAlone);

            if (useTns)
            {
                setTnsInfo(tnsAddr);
            }
        }
        catch(const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not start ezmq plus", __func__);
            throw EZMQX::Exception("Could not start ezmq plus", EZMQX::UnKnownState);
        }
    }
    // mutex unlock

    return;
}

void EZMQX::Config::initialize(EZMQX::ModeOption configMode)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

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

    return;
}

void EZMQX::Config::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(DEBUG, TAG, "%s Set TNS address %s", __func__, remoteAddr.c_str());
    ctx->setTnsInfo(remoteAddr);
    // mutex unlock
}

std::list<std::string> EZMQX::Config::addAmlModel(const std::list<std::string>& amlFilePath)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!initialized.load())
        {
            throw EZMQX::Exception("Could not add amlmodel not initialized", EZMQX::NotInitialized);
        }

        return ctx->addAmlRep(amlFilePath);
    }
    // mutex unlock

}

void EZMQX::Config::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    ctx->terminate();

}

void EZMQX::Config::reset()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        try
        {
            // terminate
            EZMQX_LOG_V(DEBUG, TAG, "%s Try terminate", __func__);
            ctx->terminate();
        }
        catch(const EZMQX::Exception& e)
        {
            throw e;
        }
        catch(...)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Could not reset ezmq plus", __func__);
            throw EZMQX::Exception("Could not reset ezmq plus", EZMQX::UnKnownState);
        }

        // clear config
        EZMQX_LOG_V(DEBUG, TAG, "%s Clear config info", __func__);
        initialized.store(false);
    }

    return;
}