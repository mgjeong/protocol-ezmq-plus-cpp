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

//Aml Test
TEST_F(SubDockerTest, AmlSubFailTest)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;
    EZMQX::AmlSubscriber* sub2 = nullptr;
    EZMQX::AmlSubscriber* sub3 = nullptr;
    EZMQX::AmlSubscriber* sub4 = nullptr;


    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSubscriber("T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::AmlSubscriber::getSubscriber("T/A", false, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub3 = EZMQX::AmlSubscriber::getSubscriber("/T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub4 = EZMQX::AmlSubscriber::getSubscriber("/T/A", false, subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }

    if (sub3)
    {
        delete sub3;
    }

    if (sub4)
    {
        delete sub4;
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

//Xml Test

TEST_F(SubDockerTest, XmlSubFailTest)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;
    EZMQX::XmlSubscriber* sub2 = nullptr;
    EZMQX::XmlSubscriber* sub3 = nullptr;
    EZMQX::XmlSubscriber* sub4 = nullptr;


    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSubscriber("T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub2 = EZMQX::XmlSubscriber::getSubscriber("T/A", false, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub3 = EZMQX::XmlSubscriber::getSubscriber("/T/A", true, subCb, errCb), EZMQX::Exception);
    EXPECT_THROW(sub4 = EZMQX::XmlSubscriber::getSubscriber("/T/A", false, subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        delete sub1;
    }

    if (sub2)
    {
        delete sub2;
    }

    if (sub3)
    {
        delete sub3;
    }

    if (sub4)
    {
        delete sub4;
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