#ifndef EZMQ_X_ENDPOINT_H
#define EZMQ_X_ENDPOINT_H

#include<string>

namespace EZMQX {

/**
* @class Endpoint
* This class Contains the APIs related with ip and port.
*
*/
class Endpoint
{
    private:
        std::string addr;
        int port;
    public:
        /**
        * Basic Constructor
        */
        Endpoint();

        /**
        * Overloaded Constructor
        * 
        * @param address ip or service name.
        *
        */
        Endpoint(std::string address);

        /**
        * Overloaded Constructor
        * 
        * @param address String ip address.
        * @param port numeric port information.
        *
        */
        Endpoint(std::string address, int port);

        /**
        * Return stored string ip address.
        * 
        * @return string ip address.
        */
        std::string getAddr();

        /**
        * Return stored port.
        * 
        * @return port.
        */
        int getPort();

        /**
        * Return ip address with port as string
        * 
        * @return ip address with port as string
        */
        std::string toString();
};

} // namespace EZMQX


#endif //EZMQ_X_ENDPOINT_H