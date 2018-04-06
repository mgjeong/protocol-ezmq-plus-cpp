#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <EZMQXAMLSubscriber.h>
#include <condition_variable>
#include <signal.h>

bool isStarted;
std::mutex m_mutex;
std::condition_variable m_cv;

void printAMLData(AML::AMLData amlData, int depth)
{
    std::string indent;
    for (int i = 0; i < depth; i++) indent += "    ";

    std::cout << indent << "{" << std::endl;

    std::vector<std::string> keys = amlData.getKeys();
    for (std::string key : keys)
    {
        std::cout << indent << "    \"" << key << "\" : ";

        AML::AMLValueType type = amlData.getValueType(key);
        if (AML::AMLValueType::String == type)
        {
            std::string valStr = amlData.getValueToStr(key);
            std::cout << valStr;
        }
        else if (AML::AMLValueType::StringArray == type)
        {
            std::vector<std::string> valStrArr = amlData.getValueToStrArr(key);
            std::cout << "[";
            for (std::string val : valStrArr)
            {
                std::cout << val;
                if (val != valStrArr.back()) std::cout << ", ";
            }
            std::cout << "]";
        }
        else if (AML::AMLValueType::AMLData == type)
        {
            AML::AMLData valAMLData = amlData.getValueToAMLData(key);
            std::cout << std::endl;
            printAMLData(valAMLData, depth + 1);
        }

        if (key != keys.back()) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << indent << "}";
}

void printAMLObject(AML::AMLObject amlObj)
{
    std::cout << "{" << std::endl;
    std::cout << "    \"device\" : " << amlObj.getDeviceId() << "," << std::endl;
    std::cout << "    \"timestamp\" : " << amlObj.getTimeStamp() << "," << std::endl;
    std::cout << "    \"id\" : " << amlObj.getId() << "," << std::endl;

    std::vector<std::string> dataNames = amlObj.getDataNames();

    for (std::string n : dataNames)
    {
        AML::AMLData data = amlObj.getData(n);

        std::cout << "    \"" << n << "\" : " << std::endl;
        printAMLData(data, 1);
        if (n != dataNames.back()) std::cout << "," << std::endl;
    }

    std::cout << "\n}" << std::endl;
}

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
    std::cout<<"Enter topic ex) /test/"<<std::endl;
    std::cin>>topic;

    try
    {
        // get config class instance & add aml model file path
        std::list<std::string> amlPath(1, "sample_data_model.aml");
        std::shared_ptr<EZMQX::Config> config(new EZMQX::Config(EZMQX::FullFeature));
        //std::shared_ptr<EZMQX::Config> config(new EZMQX::Config(EZMQX::StandAlone));

        //config->setHostInfo("TestSubscriber", "10.113.77.33");
        //config->setTnsInfo("10.113.65.174:8323");
        std::list<std::string> amlId = config->addAmlModel(amlPath);

        // error callback
        // typedef std::function<void(std::string topic, const AML::AMLObject& payload)> SubCb;
        // typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;
        EZMQX::AmlSubCb subCb = [](std::string topic, const AML::AMLObject& payload){std::cout << "subCb called" << std::endl; printAMLObject(payload);};
        EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl;};

        // create subscriber with test topic
        EZMQX::Endpoint ep("localhost", 4000);

        if (amlId.empty())
        {
            std::cout<<"Could parse any aml model"<<std::endl;
            return -1;
        }

        std::cout<<"amlId: " << amlId.front() << std::endl;
        //EZMQX::Topic knownTopic("/test/A/", amlId.front(), ep);
        //std::shared_ptr<EZMQX::AmlSubscriber> subscriber(EZMQX::AmlSubscriber::getSubscriber(knownTopic, subCb, errCb));
        std::shared_ptr<EZMQX::AmlSubscriber> subscriber(EZMQX::AmlSubscriber::getSubscriber(topic, subCb, errCb));

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