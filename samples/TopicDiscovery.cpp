#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXTopicDiscovery.h>
#include <EZMQXException.h>

int main()
{
    std::shared_ptr<EZMQX::Config> config(new EZMQX::Config(EZMQX::Docker));
    // std::shared_ptr<EZMQX::Config> config(new EZMQX::Config(EZMQX::StandAlone));
    // config->setHostInfo("TestPublisher", "10.113.77.33");
    // config->setTnsInfo("10.113.65.174");
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
            result = discovery->query(topic);

            for (auto itr = result.begin(); itr != result.end(); itr++)
            {
                std::cout<<"========================================"<<std::endl;
                EZMQX::Topic tmp = *itr;
                std::cout<<"Topic: "<<tmp.getTopic()<<std::endl;
                std::cout<<"Endpoint: "<<tmp.getEndpoint().toString()<<std::endl;
                std::cout<<"Schema: "<<tmp.getSchema()<<std::endl;
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