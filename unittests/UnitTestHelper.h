#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>
#include <gtest.h>
#include <gmock.h>

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