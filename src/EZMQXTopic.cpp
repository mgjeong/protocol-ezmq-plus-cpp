#include <EZMQXTopic.h>

EZMQX::Topic::Topic(std::string &topic, std::string &schema, EZMQX::Endpoint &endpoint): endpoint(endpoint)
{

}

std::string EZMQX::Topic::getEndpoint()
{
    return "";
}

std::string EZMQX::Topic::getSchema()
{
    return "";
}

std::string EZMQX::Topic::getTopic()
{
    return "";
}