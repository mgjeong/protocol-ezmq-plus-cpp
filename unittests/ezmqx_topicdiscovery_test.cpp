#include <gtest.h>
#include <gmock.h>
#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <string>
#include <list>

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

TEST_F(StandAloneDiscoveryTest, WithOutMock)
{
    EXPECT_THROW(mock.query(""), EZMQX::Exception);
    EXPECT_THROW(mock.query("////"), EZMQX::Exception);
    EXPECT_THROW(mock.query("/aaa"), EZMQX::Exception);
}

TEST_F(StandAloneDiscoveryTest, MockFuncCallTest)
{
    EXPECT_CALL(mock, verifyTopic(_, _, _))
    .Times(1);

    try
    {
        setDummyTns();
        mock.query("/abc");
    }
    catch(...)
    {

    }   
}

TEST_F(StandAloneDiscoveryTest, MockQueryTest)
{
    EXPECT_CALL(mock, verifyTopic(_, _, _))
    .Times(1)
    .WillOnce(SetArgReferee<1>(getDummyTopics()));

    setDummyTns();
    std::list<EZMQX::Topic> result = mock.hierarchicalQuery("/TEST");
    EXPECT_TRUE(result.size() == 3);

    EZMQX::Topic first = result.front();
    EXPECT_TRUE(first.getTopic().compare("/TEST/A") == 0);
    EXPECT_TRUE(first.getSchema().compare("dummy1") == 0);
    EXPECT_TRUE(first.getEndpoint().toString().compare("8.8.8.8:1") == 0);
    result.pop_front();

    EZMQX::Topic second = result.front();
    EXPECT_TRUE(second.getTopic().compare("/TEST/B") == 0);
    EXPECT_TRUE(second.getSchema().compare("dummy2") == 0);
    EXPECT_TRUE(second.getEndpoint().toString().compare("8.8.8.8:2") == 0);
    result.pop_front();

    EZMQX::Topic third = result.front();
    EXPECT_TRUE(third.getTopic().compare("/TEST/C") == 0);
    EXPECT_TRUE(third.getSchema().compare("dummy3") == 0);
    EXPECT_TRUE(third.getEndpoint().toString().compare("8.8.8.8:3") == 0);
    result.pop_front();
}

TEST_F(DockerDiscoveryTest, DockerMockTest)
{
    EXPECT_CALL(mock, verifyTopic(_, _, _))
    .Times(1)
    .WillOnce(SetArgReferee<1>(getDummyTopics()));

    std::list<EZMQX::Topic> result = mock.hierarchicalQuery("/TEST");
    EXPECT_TRUE(result.size() == 3);

    EZMQX::Topic first = result.front();
    EXPECT_TRUE(first.getTopic().compare("/TEST/A") == 0);
    EXPECT_TRUE(first.getSchema().compare("dummy1") == 0);
    EXPECT_TRUE(first.getEndpoint().toString().compare("8.8.8.8:1") == 0);
    result.pop_front();

    EZMQX::Topic second = result.front();
    EXPECT_TRUE(second.getTopic().compare("/TEST/B") == 0);
    EXPECT_TRUE(second.getSchema().compare("dummy2") == 0);
    EXPECT_TRUE(second.getEndpoint().toString().compare("8.8.8.8:2") == 0);
    result.pop_front();

    EZMQX::Topic third = result.front();
    EXPECT_TRUE(third.getTopic().compare("/TEST/C") == 0);
    EXPECT_TRUE(third.getSchema().compare("dummy3") == 0);
    EXPECT_TRUE(third.getEndpoint().toString().compare("8.8.8.8:3") == 0);
    result.pop_front();
}

TEST_F(FakeDiscoveryTest, FakeRest)
{
    std::list<EZMQX::Topic> topics = discovery.hierarchicalQuery("/FakeDiscoveryTest");


    std::list<EZMQX::Topic> checkList = getDummyTopics();

    auto checker = checkList.begin();
    for (auto itr =  topics.begin();  itr != topics.end(); itr++, checker++)
    {
        EXPECT_TRUE((*itr).getTopic().compare((*checker).getTopic())==0);
        EXPECT_TRUE((*itr).getSchema().compare((*checker).getSchema())==0);
        EXPECT_TRUE((*itr).getEndpoint().toString().compare((*checker).getEndpoint().toString()) == 0);
    }

}