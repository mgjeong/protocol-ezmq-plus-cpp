#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <EZMQXXMLSubscriber.h>
#include <condition_variable>
#include <signal.h>

bool isStarted;
std::mutex m_mutex;
std::condition_variable m_cv;

void sigint(int /*signal*/)
{
    if (isStarted)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.notify_all();
    }
    else
    {
        exit (EXIT_FAILURE);
    }
}

int main()
{
    isStarted = false;
    //this handler is added to check stop API
    signal(SIGINT, sigint);

    std::string topic;
    std::cout<<"Enter topic ex) /test/A"<<std::endl;
    std::cin>>topic;

    try
    {
        // get config class instance & add aml model file path
        std::list<std::string> amlPath(1, "sample_data_model.aml");
        std::list<std::string> amlId(1);
        std::shared_ptr<EZMQX::Config> config(new EZMQX::Config(EZMQX::Docker));
        //config->setHostInfo("TestSubscriber", "10.113.77.33");
        //config->setTnsInfo("10.113.65.174:8323");
        amlId = config->addAmlModel(amlPath);

        // error callback
        // typedef std::function<void(std::string topic, const AMLObject& payload)> SubCb;
        // typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;
        EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl; std::cout << payload << std::endl;};
        EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl;};

        // create subscriber with test topic
        // EZMQX::Endpoint ep("localhost", 4000);

        if (amlId.empty())
        {
            std::cout<<"Could parse any aml model"<<std::endl;
            return -1;
        }

        std::cout<<"amlId: " << amlId.front() << std::endl;
        //EZMQX::Topic knownTopic("/test/A/", amlId.front(), ep);
        //std::shared_ptr<EZMQX::XmlSubscriber> subscriber = EZMQX::XmlSubscriber::getSubscriber(knownTopic, subCb, errCb);
        std::shared_ptr<EZMQX::XmlSubscriber> subscriber(EZMQX::XmlSubscriber::getSubscriber(topic, subCb, errCb));

        std::cout<<"subscriber created"<<std::endl;

        // push to blocked
        std::cout<<"push main thread to blocked"<<std::endl;
        isStarted = true;
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock);

        // condition check
        if (!subscriber->isTerminated())
        {
            std::cout << "terminate subscriber" << std::endl;
            subscriber->terminate();
        }

        // occur exception
        subscriber->terminate();
    }
    catch(EZMQX::Exception &e)
    {
        // catch terminated exception
        std::cout << "catch exception" << std::endl;
        std::cout << e.what() << std::endl;
    }

    std::cout << "done" << std::endl;
}