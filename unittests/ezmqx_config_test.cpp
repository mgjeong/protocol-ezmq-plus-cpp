#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <gtest.h>

TEST(Config, startDockerModeWithFake)
{
    EZMQX::FakeSingletonAccessor::setFake();
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startDockerMode();

    config->reset();
}

TEST(Config, startDockerModeWithRestStub)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    // ToDo : enable stub for below rest functions
    // EZMQX::RestService::Get(NODE+PREFIX+API_CONFIG)
    // EZMQX::RestService::Get(NODE+PREFIX+API_APPS)
    // EZMQX::RestService::Get(addr)

    EXPECT_THROW(config->startDockerMode(), EZMQX::Exception);    

    config->reset();

}

TEST(Config, startStandAloneModeWithTns)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode(true, "127.0.0.1");

    config->reset();

}

TEST(Config, startStandAloneModeWithoutTns)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode(false, "127.0.0.1");

    config->reset();

}

TEST(Config, addAmlModel)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode(false, "127.0.0.1");
    std::list<std::string> amlPath(1, "sample_data_model.aml");
    std::list<std::string> amlId = config->addAmlModel(amlPath);

    config->reset();

}