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
#include <gmock.h>
#include <string>

void printAMLData(AML::AMLData amlData, int depth)
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

void printAMLObject(AML::AMLObject amlObj)
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
    virtual void SetUp()
    {
        config = EZMQX::Config::getInstance();
        config->startStandAloneMode(false, "localhost");
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

    std::string getDummyId()
    {
        return dummyId;
    }

    EZMQX::Topic getPositiveTestTopic()
    {
        return EZMQX::Topic("/T/A", getDummyId(), EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getPositiveTestTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/T/A", getDummyId(), EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/T/B", getDummyId(), EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/T/C", getDummyId(), EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }

    EZMQX::Topic getNegativeTestTopic()
    {
        return EZMQX::Topic("/T/A", "ssss", EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getNegativeTestTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/T/A", "ssss", EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/T/B", "ssss", EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/T/C", "ssss", EZMQX::Endpoint("localhost", 4002)));
        return dummy;
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
        config->startDockerMode();
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
        return EZMQX::Topic("/T/A", getDummyId(), EZMQX::Endpoint("localhost", 4000));
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/TEST/A", getDummyId(), EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/TEST/B", getDummyId(), EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/TEST/C", getDummyId(), EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }
};

class MockAmlSubscriber : public EZMQX::AmlSubscriber
{
public:
    MockAmlSubscriber() : AmlSubscriber()
    {
    }

    ~MockAmlSubscriber()
    {

    }

    void initialize(const std::string &topic, bool isHierarchical)
    {
        Subscriber::initialize(topic, isHierarchical);
    }

    void initialize(const EZMQX::Topic &topic)
    {
        std::list<EZMQX::Topic> topics(1, topic);
        Subscriber::initialize(topics);
    }

    void initialize(const std::list<EZMQX::Topic> &topics)
    {
        Subscriber::initialize(topics);
    }

    MOCK_METHOD3(verifyTopics, void(const std::string &topic, std::list<EZMQX::Topic> &verified, bool isHierarchical));
    MOCK_METHOD1(getSession, void(EZMQX::Topic topic));
};

class DockerAmlSubscriber : public testing::Test
{
protected:
    EZMQX::Config *config;
    MockAmlSubscriber *mock;
    std::string dummyId;
    virtual void SetUp()
    {
        EZMQX::FakeSingletonAccessor::setFake();
        config = EZMQX::Config::getInstance();
        config->startDockerMode();
        std::list<std::string> amlPath(1, "sample_data_model.aml");
        std::list<std::string> amlId = config->addAmlModel(amlPath);
        dummyId = amlId.front();
        mock = new MockAmlSubscriber();
    }

    virtual void TearDown()
    {
        config->reset();
        delete mock;
        mock = nullptr;
    }

    std::string getDummyId()
    {
        return dummyId;
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/TEST/A", getDummyId(), EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/TEST/B", getDummyId(), EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/TEST/C", getDummyId(), EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }
};

class MockTopicDiscovery : public EZMQX::TopicDiscovery
{
public:
    MOCK_METHOD3(verifyTopic, void(std::string& topic, std::list<EZMQX::Topic>& topics, bool isHierarchical));
};

class StandAloneDiscoveryTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    MockTopicDiscovery *mock;
    virtual void SetUp()
    {
        config = EZMQX::Config::getInstance();
        config->startStandAloneMode(true, "localhost:48323");
        mock = new MockTopicDiscovery();
    }


    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/TEST/A", "dummy1", EZMQX::Endpoint("8.8.8.8", 1)));
        dummy.push_back(EZMQX::Topic("/TEST/B", "dummy2", EZMQX::Endpoint("8.8.8.8", 2)));
        dummy.push_back(EZMQX::Topic("/TEST/C", "dummy3", EZMQX::Endpoint("8.8.8.8", 3)));
        return dummy;
    }

    virtual void TearDown()
    {
        config->reset();
        delete mock;
        mock = nullptr;
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
        config->startDockerMode();
        discovery = new EZMQX::TopicDiscovery();
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/A/A", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/A/B", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/A/C", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4002)));
        dummy.push_back(EZMQX::Topic("/B/A", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4000)));
        dummy.push_back(EZMQX::Topic("/B/B", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4001)));
        dummy.push_back(EZMQX::Topic("/B/C", "GTC_Robot_0.0.1", EZMQX::Endpoint("localhost", 4002)));
        return dummy;
    }

    virtual void TearDown()
    {
        config->reset();
        delete discovery;
        discovery = nullptr;
    }
};

class DockerDiscoveryTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    MockTopicDiscovery *mock;
    virtual void SetUp()
    {
        EZMQX::FakeSingletonAccessor::setFake();
        config = EZMQX::Config::getInstance();
        config->startDockerMode();
        mock = new MockTopicDiscovery();
    }

    std::list<EZMQX::Topic> getDummyTopics()
    {
        std::list<EZMQX::Topic> dummy;
        dummy.push_back(EZMQX::Topic("/TEST/A", "dummy1", EZMQX::Endpoint("8.8.8.8", 1)));
        dummy.push_back(EZMQX::Topic("/TEST/B", "dummy2", EZMQX::Endpoint("8.8.8.8", 2)));
        dummy.push_back(EZMQX::Topic("/TEST/C", "dummy3", EZMQX::Endpoint("8.8.8.8", 3)));
        return dummy;
    }

    virtual void TearDown()
    {
        config->reset();
        delete mock;
        mock = nullptr;
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
        config->startStandAloneMode(false, "");
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