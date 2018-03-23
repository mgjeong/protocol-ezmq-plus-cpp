#include<EZMQXEndpoint.h>

EZMQX::Endpoint::Endpoint() : addr(), port(-1)
{

}

EZMQX::Endpoint::Endpoint(std::string address) : addr(), port(-1)
{
    std::size_t pos = address.find(":");

    if (pos == std::string::npos)
    {
        addr = address;
    }
    else
    {
        try
        {
            addr = address.substr(0, pos);
            port = std::stoi(address.substr(pos+1));
        }
        catch(...)
        {
            addr = address;
        }
    }
}

EZMQX::Endpoint::Endpoint(std::string address, int port) : addr(address), port(port)
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

std::string EZMQX::Endpoint::toString()
{
    return port == -1 ? addr : addr + ":" + std::to_string(port);
}