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


#ifndef EXMQ_X_TOPIC_H
#define EXMQ_X_TOPIC_H

#include <string>
#include <EZMQXEndpoint.h>

namespace EZMQX {
/**
* @class Topic
* This class Contains the APIs that describe topic name, data model, endpoint.
*
*/
class Topic
{
    private:
        std::string name;
        std::string datamodel;
        bool secured;
        EZMQX::Endpoint endpoint;
        
    public:
        /**
        * Constructor
        */
        Topic();

        /**
        * Overloaded Constructor
        *
        * @param name name of topic.
        * @param datamodel String formatted ID of data model.
        * @param endpoint endpoint.
        */
        Topic(std::string name, std::string datamodel, bool secured, EZMQX::Endpoint endpoint);

        /**
        * Return Endpoint instance of topic.
        *
        * @return Endpoint instance of topic.
        */
        EZMQX::Endpoint getEndpoint();

        /**
        * Return Datamodel ID.
        *
        * @return Datamodel ID.
        */
        std::string getDatamodel();

        /**
        * Return name of topic.
        *
        * @return name of topic.
        */
        std::string getName();

        /**
        * Return true if topic is secured.
        *
        * @return true if topic is secured.
        */
        bool isSecured() const;
};

} //namespace EZMQX


#endif //EXMQ_X_TOPIC_H