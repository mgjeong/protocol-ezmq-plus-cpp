#include <EZMQXTopic.h>

EZMQX::Topic::Topic()
{

}

EZMQX::Topic::Topic(std::string topic, std::string schema, EZMQX::Endpoint endpoint): endpoint(endpoint), topic(topic), schema(schema)
{

}

EZMQX::Endpoint EZMQX::Topic::getEndpoint()
{
    return endpoint;
}

std::string EZMQX::Topic::getSchema()
{
    return schema;
}

std::string EZMQX::Topic::getTopic()
{
    return topic;
}