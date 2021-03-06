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


#include <gtest.h>
#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <string>

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

TEST_F(SubStandAloneTest, getUnsecuredAmlSubscriberWithSecuredTopic)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSubscriber(getSecuredTopic(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithSecuredTopic)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(getSecuredTopic(), getServerPublicKey(), getclientPublicKey(), getclientPrivateKey(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithUnsecuredTopic)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(getUnsecuredTopic(), getServerPublicKey(), getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithWrongKey)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(getSecuredTopic(), "THIS_IS_WRONG_KEY", getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithMap)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), getServerPublicKey()));

    sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithMapAndUnsecuredTopic)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getUnsecuredTopic(), getServerPublicKey()));

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithMapAndWrongServerKey)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), "THIS_IS_WRONG_KEY"));

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredAmlSubscriberWithMapAndWrongClientKey)
{
    EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl << "topic: " << topic << std::endl; printAMLObject(payload);};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::AmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), getServerPublicKey()));

    EXPECT_THROW(sub1 = EZMQX::AmlSubscriber::getSecuredSubscriber(topics, "THIS_IS_WRONG_KEY", getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
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

TEST(SubscriberTest, TerminateTest)
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startStandAloneMode("localhost", false, "127.0.0.1");

    std::list<std::string> amlPath(1, "sample_data_model.aml");
    std::list<std::string> amlIds(1);
    amlIds = config->addAmlModel(amlPath);
    std::string amlId = amlIds.front();

    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = EZMQX::XmlSubscriber::getSubscriber(EZMQX::Topic("/T/A", amlId, false, EZMQX::Endpoint("localhost", 4000)), subCb, errCb);

    config->reset();

    if (sub1)
    {
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getUnsecuredXmlSubscriberWithSecuredTopic)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSubscriber(getSecuredTopic(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithSecuredTopic)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(getSecuredTopic(), getServerPublicKey(), getclientPublicKey(), getclientPrivateKey(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithUnsecuredTopic)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(getUnsecuredTopic(), getServerPublicKey(), getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithWrongKey)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(getSecuredTopic(), "THIS_IS_WRONG_KEY", getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithMap)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), getServerPublicKey()));


    sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithMapAndUnsecuredTopic)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getUnsecuredTopic(), getServerPublicKey()));

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithMapAndWrongServerKey)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), "THIS_IS_WRONG_KEY"));

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(topics, getclientPublicKey(), getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}

TEST_F(SubStandAloneTest, getSecuredXmlSubscriberWithMapAndWrongClientKey)
{
    EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl << "topic: " <<  topic << std::endl << payload << std::endl; std::cout << payload << std::endl;};
    EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl << "topic: " <<  topic << std::endl << "err: " << errCode << std::endl;};

    EZMQX::XmlSubscriber* sub1 = nullptr;

    std::map<EZMQX::Topic, std::string> topics;

    topics.insert(std::pair<EZMQX::Topic, std::string>(getSecuredTopic(), getServerPublicKey()));

    EXPECT_THROW(sub1 = EZMQX::XmlSubscriber::getSecuredSubscriber(topics, "THIS_IS_WRONG_KEY", getclientPrivateKey(), subCb, errCb), EZMQX::Exception);

    if (sub1)
    {
        sub1->isTerminated();
        delete sub1;
    }
}