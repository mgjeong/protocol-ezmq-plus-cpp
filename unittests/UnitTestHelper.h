#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>
#include <gtest.h>
#include <gmock.h>
#include <string>

class MockTopicDiscovery : public EZMQX::TopicDiscovery
{
public:
    MOCK_METHOD2(verifyTopic, void(std::string& topic, std::list<EZMQX::Topic>& topics));
};

class DiscoveryTest : public testing::Test
{
protected:
    EZMQX::Config *config;
    MockTopicDiscovery mock;
    virtual void SetUp()
    {
        config = new EZMQX::Config(EZMQX::StandAlone);
    }

    void setDummyTns()
    {
        // config->setHostInfo("TestPublisher", "10.113.77.33");
        config->setTnsInfo("localhost:48323");
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
        delete config;
        config = nullptr;
    }
};

class TopicTest : public testing::Test
{
protected:
    EZMQX::Config* config;
    EZMQX::TopicDiscovery* discovery;

    virtual void SetUp()
    {
        config = new EZMQX::Config(EZMQX::StandAlone);
        discovery = new EZMQX::TopicDiscovery();
    }

    virtual void TearDown()
    {
        delete discovery;
        discovery = nullptr;
        delete config;
        config = nullptr;
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