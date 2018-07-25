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


#ifndef EZMQ_X_CONFIG_H
#define EZMQ_X_CONFIG_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <list>

namespace EZMQX {

class Context;
typedef enum
{
    StandAlone,
    Docker
}ModeOption;


/**
* @class Config
* This class Contains the APIs that initialize ,reset, set modes of EMQZX stack.
*
*/
class Config
{
    private:
        std::mutex lock;
        std::atomic_bool initialized;
        EZMQX::Context* ctx;
        void setTnsInfo(std::string remoteAddr);
        void initialize(EZMQX::ModeOption configMode, const std::string& tnsConfPath);
        void terminate();

        // make noncopyable
        Config();
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;
        ~Config();

    public:
        /**
        * Get instance of Config class.
        * @note Do not delete this pointer.
        *
        * @return Instance of Config class
        */
        static Config* getInstance();

        /**
        * Start as DockerMode that working with Pharos system.\n
        * In DockerMode, stack automatically using Tns service.
        *
        * @param tnsConfPath path to tns configuration file location.\n
        *  
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::Initialized - Stack already initialized try it after reset.\n
        * EZMQX::ServiceUnavailable - Could not initialize stack in current environment.\n
        * EZMQX::UnKnownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        */
        void startDockerMode( std::string tnsConfPath);

        /**
        * Start as StandAloneMode that working without pharos system.
        *
        * @param useTns bool value for whether to use tns.
        * @param tnsAddr address of tns located, if useTns is false this value will be ignored.
        * 
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::Initialized - Stack already initialized try it after reset.\n
        * EZMQX::ServiceUnavailable -Could not initialize stack in current environment.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        */
        void startStandAloneMode(bool useTns, std::string tnsAddr);

        /**
        * Add aml model file for publish or subscribe AML data.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::NotInitialized - Stack not initialized.\n
        * EZMQX::InvalidParam - Invalid file path.\n
        * EZMQX::InvalidAmlModel - Could not parse given AML model file.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see ErrorCode
        *
        */
        std::list<std::string> addAmlModel(const std::list<std::string>& amlFilePath);
        
        /**
        * Reset whole EZMQX stack.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        */
        void reset();

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H