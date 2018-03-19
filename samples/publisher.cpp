#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <EZMQXPublisher.h>
#include <EZMQXConfig.h>
#include <EZMQXException.h>

int main()
{
    try
    {
      // get config class instance & add aml model file path
      std::list<std::string> amlPath(1, "sample_data_model.aml");
      std::list<std::string> amlId(1);
      std::shared_ptr<EZMQX::Config> config = EZMQX::Config::getInstance();
      amlId = config->addAmlModel(amlPath);

      // create publisher with test topic
      std::shared_ptr<EZMQX::Publisher> publisher = EZMQX::Publisher::getPublisher("/test", EZMQX::AmlModelId, amlPath.front());
      
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
          publisher->publish(amlObj);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      // condition check
      if (!publisher->isTerminated())
      {
          publisher->terminate();
      }

      // occur exception
      publisher->terminate();
    }
    catch(EZMQX::Exception& e)
    {
        // catch terminated exception
        std::cout << "catch exception" << std::endl;
        std::cout << e.what() << std::endl;
    }

    std::cout << "done" << std::endl;
}
