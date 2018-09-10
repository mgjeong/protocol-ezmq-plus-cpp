/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/


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
        std::string getAddr() const;

        /**
        * Return stored port.
        * 
        * @return port.
        */
        int getPort() const;

        /**
        * Return ip address with port as string
        * 
        * @return ip address with port as string
        */
        std::string toString() const;
};

} // namespace EZMQX


#endif //EZMQ_X_ENDPOINT_H