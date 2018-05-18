#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>

int main()
{
    EZMQX::Config* config = EZMQX::Config::getInstance();

    config->startDockerMode();
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

    return 0;
}