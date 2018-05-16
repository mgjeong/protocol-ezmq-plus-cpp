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

TEST_F(FakeDiscoveryTest, FakeRest)
{
    std::list<EZMQX::Topic> topics = discovery->hierarchicalQuery("/FakeDiscoveryTest");


    std::list<EZMQX::Topic> checkList = getDummyTopics();

    auto checker = checkList.begin();
    for (auto itr =  topics.begin();  itr != topics.end(); itr++, checker++)
    {
        EXPECT_TRUE((*itr).getName().compare((*checker).getName())==0);
        EXPECT_TRUE((*itr).getDatamodel().compare((*checker).getDatamodel())==0);
        EXPECT_TRUE((*itr).getEndpoint().toString().compare((*checker).getEndpoint().toString()) == 0);
    }

}