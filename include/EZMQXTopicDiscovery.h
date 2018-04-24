#ifndef EXMQ_X_TOPIC_DISCOVERY_H
#define EXMQ_X_TOPIC_DISCOVERY_H

#include <string>
#include <mutex>
#include <EZMQXTopic.h>

namespace EZMQX {
class Context;
class TopicDiscovery
{
    private:
        std::mutex lock;
        EZMQX::Context* ctx;
        void validateTopic(std::string& topic);
        virtual void verifyTopic(std::string& topic, std::list<EZMQX::Topic>& topics, bool isHierarchical);
    public:
        EZMQX::Topic query(std::string topic);
        std::list<EZMQX::Topic> hierarchicalQuery(std::string topic);
        TopicDiscovery();
        ~TopicDiscovery();
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_DISCOVERY_H