#ifndef EXMQ_X_TOPIC_H
#define EXMQ_X_TOPIC_H

#include <string>
#include <EZMQXEndpoint.h>

namespace EZMQX {

class Topic
{
    private:
        EZMQX::Endpoint endpoint;
        std::string schema;
        std::string topic;
    public:
        Topic();
        Topic(const std::string &topic, const std::string &schema, const EZMQX::Endpoint &endpoint);
        std::string getEndpoint();
        std::string getSchema();
        std::string getTopic();
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_H