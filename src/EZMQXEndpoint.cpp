#include<EZMQXEndpoint.h>

EZMQX::Endpoint::Endpoint()
{

}

EZMQX::Endpoint::Endpoint(const std::string addr, int port) : addr(addr), port(port)
{

}

std::string EZMQX::Endpoint::getAddr()
{
    return addr;
}

int EZMQX::Endpoint::getPort()
{
    return port;
}