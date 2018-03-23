#ifndef EZMQ_X_ENDPOINT_H
#define EZMQ_X_ENDPOINT_H

#include<string>

namespace EZMQX {

class Endpoint
{
    private:
        std::string addr;
        int port;
    public:
        Endpoint();
        Endpoint(std::string address);
        Endpoint(std::string address, int port);
        std::string getAddr();
        int getPort();
        std::string toString();
};

} // namespace EZMQX


#endif //EZMQ_X_ENDPOINT_H