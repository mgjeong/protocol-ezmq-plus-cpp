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


#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <gtest.h>

TEST(Config, startDockerModeWithFake)
{
    EZMQX::FakeSingletonAccessor::setFake();
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startDockerMode("tnsConf.json");

    config->reset();
}

TEST(Config, startDockerModeWithRestStub)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    // ToDo : enable stub for below rest functions
    // EZMQX::RestService::Get(NODE+PREFIX+API_CONFIG)
    // EZMQX::RestService::Get(NODE+PREFIX+API_APPS)
    // EZMQX::RestService::Get(addr)

    EXPECT_THROW(config->startDockerMode("tnsConf.json"), EZMQX::Exception);    

    config->reset();

}

TEST(Config, startStandAloneModeWithTns)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode("localhost", true, "127.0.0.1");

    config->reset();

}

TEST(Config, startStandAloneModeWithoutTns)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode("localhost", false, "127.0.0.1");

    config->reset();

}

TEST(Config, addAmlModel)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode("localhost", false, "127.0.0.1");
    std::list<std::string> amlPath(1, "sample_data_model.aml");
    std::list<std::string> amlId = config->addAmlModel(amlPath);

    config->reset();

}