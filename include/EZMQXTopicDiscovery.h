#ifndef EXMQ_X_TOPIC_DISCOVERY_H
#define EXMQ_X_TOPIC_DISCOVERY_H

#include <string>
#include <mutex>
#include <EZMQXTopic.h>

namespace EZMQX {
class Context;

/**
* @class TopicDiscovery
* This class Contains the APIs that query topic to Tns Server.
*
*/
class TopicDiscovery
{
    private:
        std::mutex lock;
        EZMQX::Context* ctx;
        void validateTopic(std::string& topic);
        void verifyTopic(std::string& topic, std::list<EZMQX::Topic>& topics, bool isHierarchical);
    public:
        /**
        * Return instance of topic that related with given topic.
        *
        * @param Name of topic that will be queried to Tns server.
        *
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * @see EZMQX::Exception
        *
        * @return instance of topic.
        *
        */
        EZMQX::Topic query(std::string topic);

        /**
        * Return list of instance of topic that related with given topic.
        *
        * @param Name of topic that will be queried to Tns server with hierarchical query option.
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::Terminated - Stack terminated.\n
        * EZMQX::InvalidTopic - Topic validation fail.\n
        * EZMQX::RestError - Rest error, see detail on contained message.\n
        * EZMQX::TnsNotAvailable - Tns service not available, use DockerMode or set Tns information for StandAloneMode.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        * @return list of instance of topic.
        *
        */
        std::list<EZMQX::Topic> hierarchicalQuery(std::string topic);
        
        /**
        * Constructor
        *
        * @throws EZMQX::Exception thrown with ErrorCode, See below for detail.\n
        * EZMQX::Terminated - Stack terminated.\n
        * EZMQX::InvalidTopic - Topic validation fail.\n
        * EZMQX::RestError - Rest error, see detail on contained message.\n
        * EZMQX::TnsNotAvailable - Tns service not available, use DockerMode or set Tns information for StandAloneMode.\n
        * EZMQX::UnknownState - Unknown reason.\n
        *
        * @see EZMQX::Exception
        *
        */
        TopicDiscovery();

        /**
        * Destructor
        */
        ~TopicDiscovery();
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_DISCOVERY_H