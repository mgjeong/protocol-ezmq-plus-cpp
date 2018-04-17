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

TEST_F(DiscoveryTest, WithTns)
{
    EXPECT_CALL(mock, verifyTopic(_, _))
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

TEST_F(DiscoveryTest, WithTns2)
{
    EXPECT_CALL(mock, verifyTopic(_, _))
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