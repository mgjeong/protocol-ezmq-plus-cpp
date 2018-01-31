#ifndef EZMQ_X_ENDPOINT_H
#define EZMQ_X_ENDPOINT_H

#include<string>

namespace EZMQX {

class Endpoint
{
    private:
        std::string addr;
        int port;
        Endpoint();
    public:
        Endpoint(std::string addr, int port);
        std::string getAddr();
        int getPort();
};

} // namespace EZMQX


#endif //EZMQ_X_ENDPOINT_H