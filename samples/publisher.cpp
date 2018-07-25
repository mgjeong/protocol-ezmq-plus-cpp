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
#include <thread>
#include <chrono>
#include <vector>
#include <EZMQXAmlPublisher.h>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <AMLException.h>
#include <thread>
#include <condition_variable>

void printAMLData(const AML::AMLData& amlData, int depth)
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

void printAMLObject(const AML::AMLObject& amlObj)
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

EZMQX::AmlPublisher *publisherA = nullptr;
EZMQX::AmlPublisher *publisherB = nullptr;
EZMQX::AmlPublisher *publisherC = nullptr;
std::thread *terminater = nullptr;
std::condition_variable timerCond;
std::mutex timerMutex;

void deleteTopic()
{
  std::cout<<"entered deleteToopic"<<std::endl;
  std::unique_lock<std::mutex> timerLock(timerMutex);
  timerCond.wait_for(timerLock, std::chrono::minutes(7));

  std::cout<<"try delete"<<std::endl;
  delete publisherA;
  delete publisherB;
  delete publisherC;
  std::cout<<"deleted"<<std::endl;
  return;
}

int main()
{
  std::string topic;
  std::cout<<"Enter topic ex) /test/"<<std::endl;
  std::cin>>topic;
    try
    {
      // get config class instance & add aml model file path
      std::list<std::string> amlPath(1, "sample_data_model.aml");
      std::list<std::string> amlId(1);
      EZMQX::Config* config = EZMQX::Config::getInstance();

      config->startDockerMode("tnsConf.json");
      //config->startStandAloneMode(true, "10.113.66.234");

      amlId = config->addAmlModel(amlPath);

      // create publisher with test topic
      publisherA = EZMQX::AmlPublisher::getPublisher(topic + "A", EZMQX::AmlModelId, amlId.front(), 4000);
      publisherB = EZMQX::AmlPublisher::getPublisher(topic + "B", EZMQX::AmlModelId, amlId.front(), 4001);
      publisherC = EZMQX::AmlPublisher::getPublisher(topic + "C", EZMQX::AmlModelId, amlId.front(), 4002);

      // create AMLObject
      std::string deviceId = "GTC001";
      std::string timeStamp = "123456789";

      AML::AMLObject amlObj(deviceId, timeStamp);

      // create "Model" data
      AML::AMLData model;
      model.setValue("ctname", "Model_107.113.97.248");
      model.setValue("con", "SR-P7-970");

      // create "Sample" data
      AML::AMLData axis;
      axis.setValue("x", "20");
      axis.setValue("y", "110");
      axis.setValue("z", "80");

      AML::AMLData info;
      info.setValue("id", "f437da3b");
      info.setValue("axis", axis);

      std::vector<std::string> appendix;
      appendix.push_back("52303");
      appendix.push_back("935");
      appendix.push_back("1442");

      AML::AMLData sample;
      sample.setValue("info", info);
      sample.setValue("appendix", appendix);

      // Add Datas to AMLObject
      amlObj.addData("Model", model);
      amlObj.addData("Sample", sample);

      //terminater = new std::thread(deleteTopic);

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

    }
    catch(EZMQX::Exception& e)
    {
        std::cout << "catch EXMQX exception" << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch (AML::AMLException& e)
    {
        std::cout << "catch AML exception" << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "catch exception" << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "unknown exception" << std::endl;
    }

    std::cout << "done" << std::endl;
}