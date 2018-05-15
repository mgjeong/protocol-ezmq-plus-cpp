#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <gtest.h>
#include <gmock.h>

using testing::Action;
using testing::ActionInterface;
using testing::Assign;
using testing::ByMove;
using testing::ByRef;
using testing::DefaultValue;
using testing::DoDefault;
using testing::IgnoreResult;
using testing::Invoke;
using testing::InvokeWithoutArgs;
using testing::MakePolymorphicAction;
using testing::Ne;
using testing::PolymorphicAction;
using testing::Return;
using testing::ReturnNull;
using testing::ReturnRef;
using testing::ReturnRefOfCopy;
using testing::SetArgPointee;
using testing::SetArgumentPointee;
using testing::Unused;
using testing::_;
using testing::AtLeast;
using testing::get;
using testing::internal::BuiltInDefaultValue;
using testing::internal::Int64;
using testing::internal::UInt64;
using testing::make_tuple;
using testing::tuple;
using testing::tuple_element;
using testing::SetArgReferee;

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