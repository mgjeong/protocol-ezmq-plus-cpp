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


#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>

int main()
{
    try
    {
        EZMQX::Config* config = EZMQX::Config::getInstance();

        config->startDockerMode("tnsConf.json");
        //config->startStandAloneMode(true, "10.113.65.174");

        std::shared_ptr<EZMQX::TopicDiscovery> discovery(new EZMQX::TopicDiscovery());
        std::list<EZMQX::Topic> result;

        while (1)
        {
            std::string topic;
            std::cout<<"Enter topic ex) /TEST/A"<<std::endl;
            std::cin>>topic;

            try
            {
                if (!result.empty())
                {
                    result.clear();
                }
                result = discovery->hierarchicalQuery(topic);

                for (auto itr = result.begin(); itr != result.end(); itr++)
                {
                    std::cout<<"========================================"<<std::endl;
                    EZMQX::Topic tmp = *itr;
                    std::cout<<"Topic: "<<tmp.getName()<<std::endl;
                    std::cout<<"Endpoint: "<<tmp.getEndpoint().toString()<<std::endl;
                    std::cout<<"Schema: "<<tmp.getDatamodel()<<std::endl;
                }
                std::cout<<"========================================"<<std::endl;
            }
            catch (const EZMQX::Exception& e)
            {
                std::cout<<"Catch Exception: "<<e.what()<<std::endl;
            }
            catch (...)
            {
                std::cout<<"Unknown exception throwed"<<std::endl;
            }
        }
    }
    catch (const EZMQX::Exception& e)
    {
        std::cout<<"Catch Exception: "<<e.what()<<std::endl;
    }
    catch (...)
    {
        std::cout<<"Unknown exception throwed"<<std::endl;
    }

    return 0;
}