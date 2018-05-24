#ifndef EXMQ_X_TOPIC_H
#define EXMQ_X_TOPIC_H

#include <string>
#include <EZMQXEndpoint.h>

namespace EZMQX {
/**
* @class Topic
* This class Contains the APIs that describe topic name, data model, endpoint.
*
*/
class Topic
{
    private:
        std::string name;
        std::string datamodel;
        EZMQX::Endpoint endpoint;
        
    public:
        /**
        * Constructor
        */
        Topic();

        /**
        * Overloaded Constructor
        *
        * @param name name of topic.
        * @param datamodel String formatted ID of data model.
        * @param endpoint endpoint.
        */
        Topic(std::string name, std::string datamodel, EZMQX::Endpoint endpoint);

        /**
        * Return Endpoint instance of topic.
        *
        * @return Endpoint instance of topic.
        */
        EZMQX::Endpoint getEndpoint();

        /**
        * Return Datamodel ID.
        *
        * @return Datamodel ID.
        */
        std::string getDatamodel();

        /**
        * Return name of topic.
        *
        * @return name of topic.
        */
        std::string getName();
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_H