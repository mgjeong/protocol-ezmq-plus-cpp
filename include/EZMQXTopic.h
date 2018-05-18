#ifndef EXMQ_X_TOPIC_H
#define EXMQ_X_TOPIC_H

#include <string>
#include <EZMQXEndpoint.h>

namespace EZMQX {

class Topic
{
    private:
        std::string name;
        std::string datamodel;
        EZMQX::Endpoint endpoint;
        
    public:
        Topic();
        Topic(std::string name, std::string datamodel, EZMQX::Endpoint endpoint);
        EZMQX::Endpoint getEndpoint();
        std::string getDatamodel();
        std::string getName();
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_H