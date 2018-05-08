#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXAMLSubscriber.h>
#include <EZMQXAmlPublisher.h>
#include <EZMQXException.h>
#include <EZMQXFakeSingletonAccessor.h>
#include <EZMQXFakeRestAccessor.h>
#include <gtest.h>
#include <gmock.h>
#include <string>

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

class MockAmlPublisher : public EZMQX::AmlPublisher
{
public:
    MOCK_METHOD1(registerTopic, void(EZMQX::Topic& topic));
};

class DockerAmlPublisher : public testing::Test
{
protected:
    EZMQX::Config *config;
    MockAmlPublisher *mock;
    virtual void SetUp()
    {
        EZMQX::FakeSingletonAccessor::setFake();
        config = EZMQX::Config::getInstance();
        config->startDockerMode();
    }

    virtual void TearDown()
    {
        config->reset();
        delete mock;
        mock = nullptr;
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