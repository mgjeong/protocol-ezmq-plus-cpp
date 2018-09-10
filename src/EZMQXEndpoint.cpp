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

#include<EZMQXEndpoint.h>

static const std::string COLLON = ":";

EZMQX::Endpoint::Endpoint() : addr(), port(-1)
{

}

EZMQX::Endpoint::Endpoint(std::string address) : addr(), port(-1)
{
    std::size_t pos = address.find(COLLON);

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

std::string EZMQX::Endpoint::getAddr() const
{
    return addr;
}

int EZMQX::Endpoint::getPort() const
{
    return port;
}

std::string EZMQX::Endpoint::toString() const
{
    return port == -1 ? addr : addr + COLLON + std::to_string(port);
}