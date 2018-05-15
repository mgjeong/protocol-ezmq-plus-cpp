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

// TEST_F(DockerAmlSubscriber, ExpectThrow)
// {

//     EXPECT_CALL(*mock, verifyTopics(_, _, _))
//     .Times(2);

//     std::string topic = "/TEST";

//     EXPECT_THROW(mock->initialize(topic, true), EZMQX::Exception);
//     EXPECT_THROW(mock->initialize(topic, false), EZMQX::Exception);
// }

// TEST_F(DockerAmlSubscriber, ExpectThreeDummies)
// {

//     EXPECT_CALL(*mock, verifyTopics(_, _, _))
//     .WillOnce(SetArgReferee<1>(getDummyTopics()));

//     EXPECT_CALL(*mock, getSession(_))
//     .Times(3);

//     std::string topic = "/TEST";

//     mock->initialize(topic, true);
//     std::list<EZMQX::Topic> result = mock->getTopics();

//     EXPECT_TRUE(result.size() == 3);

//     EZMQX::Topic first = result.front();
//     EXPECT_TRUE(first.getName().compare("/TEST/A") == 0);
//     EXPECT_TRUE(first.getDatamodel().compare(getDummyId()) == 0);
//     EXPECT_TRUE(first.getEndpoint().toString().compare("localhost:4000") == 0);
//     result.pop_front();

//     EZMQX::Topic second = result.front();
//     EXPECT_TRUE(second.getName().compare("/TEST/B") == 0);
//     EXPECT_TRUE(second.getDatamodel().compare(getDummyId()) == 0);
//     EXPECT_TRUE(second.getEndpoint().toString().compare("localhost:4001") == 0);
//     result.pop_front();

//     EZMQX::Topic third = result.front();
//     EXPECT_TRUE(third.getName().compare("/TEST/C") == 0);
//     EXPECT_TRUE(third.getDatamodel().compare(getDummyId()) == 0);
//     EXPECT_TRUE(third.getEndpoint().toString().compare("localhost:4002") == 0);
//     result.pop_front();

    
// }


//Aml Test
TEST_F(SubDockerTest, AmlSubFailTest)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;
    EZMQX::AmlSubscriber* sub2 = nullptr;


    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSubscriber("T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::AmlSubscriber::getSubscriber("T/A", false, subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, AmlSubFailTest2)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;
    EZMQX::AmlSubscriber* sub2 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSubscriber(getNegativeTestTopic(), subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::AmlSubscriber::getSubscriber(getNegativeTestTopics(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, AmlSubSuccessTest1)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = EZMQX::AmlSubscriber::getSubscriber(getPositiveTestTopic(), subCb, errCb);
    EZMQX::AmlSubscriber* sub2 = EZMQX::AmlSubscriber::getSubscriber(getPositiveTestTopics(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, AmlSubActualTest1)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = EZMQX::AmlSubscriber::getSubscriber(getPositiveTestTopic(), subCb, errCb);

    pubTest();
    pubTest();
    pubTest();

    destoryPub();

    if (sub1)
    {
        delete sub1;
    }

}

//Xml Test

TEST_F(SubDockerTest, XmlSubFailTest)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;
    EZMQX::XmlSubscriber* sub2 = nullptr;


    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSubscriber("T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::XmlSubscriber::getSubscriber("T/A", false, subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, XmlSubFailTest2)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;
    EZMQX::XmlSubscriber* sub2 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSubscriber(getNegativeTestTopic(), subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::XmlSubscriber::getSubscriber(getNegativeTestTopics(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, XmlSubSuccessTest1)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = EZMQX::XmlSubscriber::getSubscriber(getPositiveTestTopic(), subCb, errCb);
    EZMQX::XmlSubscriber* sub2 = EZMQX::XmlSubscriber::getSubscriber(getPositiveTestTopics(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }
}

TEST_F(SubStandAloneTest, XmlSubActualTest1)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = EZMQX::XmlSubscriber::getSubscriber(getPositiveTestTopic(), subCb, errCb);
    sub1->getTopics();

    pubTest();
    pubTest();
    pubTest();

    destoryPub();

    if (sub1)
    {
        delete sub1;
    }

}