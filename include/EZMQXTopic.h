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
        Topic();
    public:
        Topic(std::string topic, std::string schema, EZMQX::Endpoint endpoint);
        std::string getEndpoint();
        std::string getSchema();
        std::string getTopic();
}


} //namespace EZMQX


#endif //EXMQ_X_TOPIC_H