#include <EZMQXTopic.h>

EZMQX::Topic::Topic()
{

}

EZMQX::Topic::Topic(const std::string &topic, const std::string &schema, const EZMQX::Endpoint &endpoint): endpoint(endpoint)
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