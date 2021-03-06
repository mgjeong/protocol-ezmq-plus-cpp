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
#include <EZMQXTopicDiscovery.h>
#include <EZMQXAMLSubscriber.h>
#include <EZMQXXMLSubscriber.h>
#include <EZMQXAmlPublisher.h>
#include <EZMQXException.h>
#include <EZMQXFakeSingletonAccessor.h>
#include <EZMQXFakeRestAccessor.h>
#include <gtest.h>
#include <string>
#include <chrono>
#include <thread>

void printAMLData(const AML::AMLData& amlData, int depth)
{
    std::string indent;
    for (int i = 0; i < depth; i++) indent += "    ";

    std::cout << indent << "{" << std::endl;

    std::vector<std::string> keys = amlData.getKeys();
    for (std::string key : keys)
    {
        std::cout << indent << "    \"" << key << "\" : ";

        AML::AMLValueType type = amlData.getValueType(key);
        if (AML::AMLValueType::String == type)
        {
            std::string valStr = amlData.getValueToStr(key);
            std::cout << valStr;
        }
        else if (AML::AMLValueType::StringArray == type)
        {
            std::vector<std::string> valStrArr = amlData.getValueToStrArr(key);
            std::cout << "[";
            for (std::string val : valStrArr)
            {
                std::cout << val;
                if (val != valStrArr.back()) std::cout << ", ";
            }
            std::cout << "]";
        }
        else if (AML::AMLValueType::AMLData == type)
        {
            AML::AMLData valAMLData = amlData.getValueToAMLData(key);
            std::cout << std::endl;
            printAMLData(valAMLData, depth + 1);
        }

        if (key != keys.back()) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << indent << "}";
}

void printAMLObject(const AML::AMLObject& amlObj)
{
    std::cout << "{" << std::endl;
    std::cout << "    \"device\" : " << amlObj.getDeviceId() << "," << std::endl;
    std::cout << "    \"timestamp\" : " << amlObj.getTimeStamp() << "," << std::endl;
    std::cout << "    \"id\" : " << amlObj.getId() << "," << std::endl;

    std::vector<std::string> dataNames = amlObj.getDataNames();

    for (std::string n : dataNames)
    {
        AML::AMLData data = amlObj.getData(n);

        std::cout << "    \"" << n << "\" : " << std::endl;
        printAMLData(data, 1);
        if (n != dataNames.back()) std::cout << "," << std::endl;
    }

    std::cout << "\n}" << std::endl;
}


class SubStandAloneTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    std::string dummyId;
    EZMQX::AmlPublisher* pub;
    std::string serverPublicKey = "tXJx&1^QE2g7WCXbF.$$TVP.wCtxwNhR8?iLi&S<";
    std::string clientPublicKey = "-QW?Ved(f:<::3d5tJ$[4Er&]6#9yr=vha/caBc(";
    std::string clientPrivateKey = "ZB1@RS6Kv^zucova$kH(!o>tZCQ.<!Q)6-0aWFmW";

    virtual void SetUp()
    {
        config = EZMQX::Config::getInstance();
        config->startStandAloneMode("localhost", false, "localhost");
        std::list<std::string> amlPath(1, "sample_data_model.aml");
        std::list<std::string> amlId = config->addAmlModel(amlPath);
        dummyId = amlId.front();

        pub = EZMQX::AmlPublisher::getPublisher("/T/A", EZMQX::AmlModelId, dummyId, 4000);
    }

    virtual void TearDown()
    {
        config->reset();

        if (pub)
        {
            delete pub;
            pub = nullptr;
        }
    }

    std::string getServerPublicKey()
    {
        return serverPublicKey;
    }

    std::string getclientPublicKey()
    {
        return clientPublicKey;
    }

    std::string getclientPrivateKey()
    {
        return clientPrivateKey;
    }

    std::string getDummyId()
    {
        return dummyId;
    }

    EZMQX::Topic getPositiveTestTopic()
    {
        return EZMQX::Topic("/T/A", getDummyId(), false, EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getPositiveTestTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/T/A", getDummyId(), false, EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/T/B", getDummyId(), false, EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/T/C", getDummyId(), false, EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }

    EZMQX::Topic getNegativeTestTopic()
    {
        return EZMQX::Topic("/T/A", "ssss", false, EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getNegativeTestTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/T/A", "ssss", false, EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/T/B", "ssss", false, EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/T/C", "ssss", false, EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }

    EZMQX::Topic getSecuredTopic()
    {
        return EZMQX::Topic("/T/A", getDummyId(), true, EZMQX::Endpoint("localhost", 4000));
    }

    EZMQX::Topic getUnsecuredTopic()
    {
        return EZMQX::Topic("/T/A", getDummyId(), false, EZMQX::Endpoint("localhost", 4000));
    }

    void destoryPub()
    {
        if (pub)
        {
            delete pub;
            pub = nullptr;
        }
    }

    void pubTest()
    {
        // create AMLObject
            std::string deviceId = "GTC001";
            std::string timeStamp = "123456789";

            AML::AMLObject amlObj(deviceId, timeStamp);

            // create "Model" data
            AML::AMLData model;
            model.setValue("ctname", "Model_107.113.97.248");
            model.setValue("con", "SR-P7-970");

            // create "Sample" data
            AML::AMLData axis;
            axis.setValue("x", "20");
            axis.setValue("y", "110");
            axis.setValue("z", "80");

            AML::AMLData info;
            info.setValue("id", "f437da3b");
            info.setValue("axis", axis);

            std::vector<std::string> appendix;
            appendix.push_back("52303");
            appendix.push_back("935");
            appendix.push_back("1442");

            AML::AMLData sample;
            sample.setValue("info", info);
            sample.setValue("appendix", appendix);

            // Add Datas to AMLObject
            amlObj.addData("Model", model);
            amlObj.addData("Sample", sample);
            if (pub)
            {
                pub->publish(amlObj);
            }

    }
};

class SubDockerTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    std::string dummyId;
    virtual void SetUp()
    {
        EZMQX::FakeSingletonAccessor::setFake();
        config = EZMQX::Config::getInstance();
        config->startDockerMode("tnsConf.json");
        std::list<std::string> amlPath(1, "sample_data_model.aml");
        std::list<std::string> amlId = config->addAmlModel(amlPath);
        dummyId = amlId.front();
    }

    virtual void TearDown()
    {
        config->reset();
    }

    std::string getDummyId()
    {
        return dummyId;
    }

    EZMQX::Topic getTestTopic()
    {
        return EZMQX::Topic("/T/A", getDummyId(), false, EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/TEST/A", getDummyId(), false, EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/TEST/B", getDummyId(), false, EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/TEST/C", getDummyId(), false, EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }
};

class FakeDiscoveryTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    EZMQX::TopicDiscovery *discovery;
    virtual void SetUp()
    {
        EZMQX::FakeSingletonAccessor::setFake();
        EZMQX::FakeRestAccessor::setFake();
        config = EZMQX::Config::getInstance();
        config->startDockerMode("tnsConf.json");
        discovery = new EZMQX::TopicDiscovery();
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/A/A", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/A/B", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/A/C", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4002)));
        dummy.push_back(EZMQX::Topic("/B/A", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/B/B", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/B/C", "GTC_Robot_0.0.1", false, EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }

    virtual void TearDown()
    {
        config->reset();
        delete discovery;
        discovery = nullptr;
    }
};

class TopicTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    EZMQX::TopicDiscovery *discovery;

    virtual void SetUp()
    {
        config = EZMQX::Config::getInstance();
        config->startStandAloneMode("localhost", false, "");
        discovery = new EZMQX::TopicDiscovery();
    }

    virtual void TearDown()
    {
        delete discovery;
        discovery = nullptr;
        config->reset();
    }

    EZMQX::ErrorCode topicTest(std::string topic)
    {
        EZMQX::ErrorCode ret = EZMQX::NotInitialized;

        try
        {
            discovery->query(topic);
        }
        catch(const EZMQX::Exception& e)
        {
            ret = e.getErrCode();
        }

        return ret;
    }
};