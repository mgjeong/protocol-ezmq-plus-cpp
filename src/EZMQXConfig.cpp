/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

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

void EZMQX::Config::startDockerMode( std::string tnsConfPath)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        try
        {
            std::lock_guard<std::mutex> scopedLock(lock);
            initialize(EZMQX::Docker, tnsConfPath);
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

void EZMQX::Config::startStandAloneMode(std::string hostAddr, bool useTns, std::string tnsAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        try
        {
            std::string empty = "";
            std::lock_guard<std::mutex> scopedLock(lock);
            initialize(EZMQX::StandAlone, empty);

            setHostInfo(hostAddr);

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

void EZMQX::Config::initialize(EZMQX::ModeOption configMode, const std::string& tnsConfPath)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    if (initialized.load())
    {
        throw EZMQX::Exception("Could not initialize already initialized", EZMQX::Initialized);
    }
    else
    {
        const std::string& tnsConfPathRef = tnsConfPath;
        if (EZMQX::StandAlone == configMode)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Set StandAlone mode", __func__);
            ctx->setStandAloneMode(true, tnsConfPath);
        }
        else if(EZMQX::Docker == configMode)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s Set as Docker", __func__);
            ctx->initialize(tnsConfPathRef);
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

void EZMQX::Config::setHostInfo(std::string addr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(DEBUG, TAG, "%s Set host address %s", __func__, addr.c_str());
    ctx->setHostInfo(LOCAL_ADDR, addr);
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
