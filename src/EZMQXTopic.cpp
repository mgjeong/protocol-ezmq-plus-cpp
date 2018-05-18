#include <EZMQXTopic.h>

EZMQX::Topic::Topic()
{

}

EZMQX::Topic::Topic(std::string name, std::string datamodel, EZMQX::Endpoint endpoint): name(name), datamodel(datamodel), endpoint(endpoint)
{

}

EZMQX::Endpoint EZMQX::Topic::getEndpoint()
{
    return endpoint;
}

std::string EZMQX::Topic::getDatamodel()
{
    return datamodel;
}

std::string EZMQX::Topic::getName()
{
    return name;
}