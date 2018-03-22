#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <EZMQXPublisher.h>
#include <EZMQXConfig.h>
#include <EZMQXException.h>

void printAMLData(AMLData amlData, int depth)
{
    std::string indent;
    for (int i = 0; i < depth; i++) indent += "    ";

    std::cout << indent << "{" << std::endl;

    std::vector<std::string> keys = amlData.getKeys();
    for (std::string key : keys)
    {
        std::cout << indent << "    \"" << key << "\" : ";

        AMLValueType type = amlData.getValueType(key);
        if (AMLValueType::String == type)
        {
            std::string valStr = amlData.getValueToStr(key);
            std::cout << valStr;
        }
        else if (AMLValueType::StringArray == type)
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
        else if (AMLValueType::AMLData == type)
        {
            AMLData valAMLData = amlData.getValueToAMLData(key);
            std::cout << std::endl;
            printAMLData(valAMLData, depth + 1);
        }

        if (key != keys.back()) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << indent << "}";
}

void printAMLObject(AMLObject amlObj)
{
    std::cout << "{" << std::endl;
    std::cout << "    \"device\" : " << amlObj.getDeviceId() << "," << std::endl;
    std::cout << "    \"timestamp\" : " << amlObj.getTimeStamp() << "," << std::endl;
    std::cout << "    \"id\" : " << amlObj.getId() << "," << std::endl;

    std::vector<std::string> dataNames = amlObj.getDataNames();

    for (std::string n : dataNames)
    {
        AMLData data = amlObj.getData(n);

        std::cout << "    \"" << n << "\" : " << std::endl;
        printAMLData(data, 1);
        if (n != dataNames.back()) std::cout << "," << std::endl;
    }

    std::cout << "\n}" << std::endl;
}

int main()
{
    try
    {
      // get config class instance & add aml model file path
      std::list<std::string> amlPath(1, "sample_data_model.aml");
      std::list<std::string> amlId(1);
      std::shared_ptr<EZMQX::Config> config = EZMQX::Config::getInstance(EZMQX::StandAlone);
      config->setHostInfo("TestPublisher", "10.113.77.33");
      config->setTnsInfo("10.113.65.174:8323");
      amlId = config->addAmlModel(amlPath);

      // create publisher with test topic
      std::shared_ptr<EZMQX::Publisher> publisherA = EZMQX::Publisher::getPublisher("/test2/A/", EZMQX::AmlModelId, amlId.front(), 4000);
      std::shared_ptr<EZMQX::Publisher> publisherB = EZMQX::Publisher::getPublisher("/test2/B/", EZMQX::AmlModelId, amlId.front(), 4001);
      std::shared_ptr<EZMQX::Publisher> publisherC = EZMQX::Publisher::getPublisher("/test2/C/", EZMQX::AmlModelId, amlId.front(), 4002);
      
      // create AMLObject
      std::string deviceId = "GTC001";
      std::string timeStamp = "123456789";

      AMLObject amlObj(deviceId, timeStamp);

      // create "Model" data
      AMLData model;
      model.setValue("ctname", "Model_107.113.97.248");
      model.setValue("con", "SR-P7-970");

      // create "Sample" data
      AMLData axis;
      axis.setValue("x", "20");
      axis.setValue("y", "110");
      axis.setValue("z", "80");

      AMLData info;
      info.setValue("id", "f437da3b");
      info.setValue("axis", axis);

      std::vector<std::string> appendix;
      appendix.push_back("52303");
      appendix.push_back("935");
      appendix.push_back("1442");

      AMLData sample;
      sample.setValue("info", info);
      sample.setValue("appendix", appendix);

      // Add Datas to AMLObject
      amlObj.addData("Model", model);
      amlObj.addData("Sample", sample);

      while(1)
      {
          // publish AMLObject
          publisherA->publish(amlObj);
          publisherB->publish(amlObj);
          publisherC->publish(amlObj);
          std::cout << "Publish!!!" << std::endl;
          printAMLObject(amlObj);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      // condition check
      if (!publisherA->isTerminated())
      {
          publisherA->terminate();
          publisherB->terminate();
          publisherC->terminate();
      }

      // occur exception
      publisherA->terminate();
    }
    catch(EZMQX::Exception& e)
    {
        // catch terminated exception
        std::cout << "catch exception" << std::endl;
        std::cout << e.what() << std::endl;
    }

    std::cout << "done" << std::endl;
}
