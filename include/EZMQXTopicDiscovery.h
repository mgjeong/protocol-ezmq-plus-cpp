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
    public:
        std::list<EZMQX::Topic> query(std::string topic);
        TopicDiscovery();
        ~TopicDiscovery();
        
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_DISCOVERY_H