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

#include <EZMQXTopic.h>

EZMQX::Topic::Topic()
 : name(""), datamodel(""), secured(false), endpoint()
{

}

EZMQX::Topic::Topic(std::string name, std::string datamodel, bool secured, EZMQX::Endpoint endpoint)
 : name(name), datamodel(datamodel), secured(secured), endpoint(endpoint)
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

bool EZMQX::Topic::isSecured() const
{
    return secured;
}