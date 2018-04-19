#include <gtest.h>
#include <gmock.h>
#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <string>

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

TEST_F(DockerAmlSubscriber, ExpectThrow)
{

    EXPECT_CALL(mock, verifyTopics(_, _))
    .Times(1);

    std::string topic = "/TEST";

    EXPECT_THROW(mock.initialize(topic), EZMQX::Exception);
}

TEST_F(DockerAmlSubscriber, ExpectThreeDummies)
{

    EXPECT_CALL(mock, verifyTopics(_, _))
    .WillOnce(SetArgReferee<1>(getDummyTopics()));

    EXPECT_CALL(mock, getSession(_, _))
    .Times(3);

    std::string topic = "/TEST";

    
    std::list<EZMQX::Topic> result;
    try
    {
        mock.initialize(topic);
        result = mock.getTopics();
    }catch(...)
    {
        std::cout << "something" << std::endl;
    }

    

    std::cout << result.size() <<std::endl;
    EXPECT_TRUE(result.size() == 3);

    EZMQX::Topic first = result.front();
    EXPECT_TRUE(first.getTopic().compare("/TEST/A") == 0);
    EXPECT_TRUE(first.getSchema().compare(getDummyId()) == 0);
    EXPECT_TRUE(first.getEndpoint().toString().compare("localhost:4000") == 0);
    result.pop_front();

    EZMQX::Topic second = result.front();
    EXPECT_TRUE(second.getTopic().compare("/TEST/B") == 0);
    EXPECT_TRUE(second.getSchema().compare(getDummyId()) == 0);
    EXPECT_TRUE(second.getEndpoint().toString().compare("localhost:4001") == 0);
    result.pop_front();

    EZMQX::Topic third = result.front();
    EXPECT_TRUE(third.getTopic().compare("/TEST/C") == 0);
    EXPECT_TRUE(third.getSchema().compare(getDummyId()) == 0);
    EXPECT_TRUE(third.getEndpoint().toString().compare("localhost:4002") == 0);
    result.pop_front();

    
}

// EXPECT_TRUE(mock != nullptr);
// EXPECT_TRUE(mock != nullptr);
// EXPECT_THROW(mock.query(""), EZMQX::Exception);
// EXPECT_THROW(mock.query("////"), EZMQX::Exception);
// EXPECT_THROW(mock.query("/aaa"), EZMQX::Exception);

// std::list<EZMQX::Topic> result = mock.getTopics();


    