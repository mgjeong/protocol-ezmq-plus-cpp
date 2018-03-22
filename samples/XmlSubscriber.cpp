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

    try
    {
      // get config class instance & add aml model file path
      std::list<std::string> amlPath(1, "sample_data_model.aml");
      std::list<std::string> amlId(1);
      std::shared_ptr<EZMQX::Config> config = EZMQX::Config::getInstance(EZMQX::StandAlone);
      config->setHostInfo("TestSubscriber", "10.113.77.33");
      config->setTnsInfo("10.113.65.174:8323");
      amlId = config->addAmlModel(amlPath);

      // error callback
      // typedef std::function<void(std::string topic, const AMLObject& payload)> SubCb;
      // typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;
      EZMQX::XmlSubCb subCb = [](std::string topic, const std::string& payload){std::cout << "subCb called" << std::endl; std::cout << payload << std::endl;};
      EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl;};
      
      // create subscriber with test topic
      // EZMQX::Endpoint ep("localhost", 4000);

      std::cout<<"subscriber created"<<std::endl;
      //EZMQX::Topic topic("/test/", amlId.front(), ep);
      std::shared_ptr<EZMQX::XmlSubscriber> subscriber = EZMQX::XmlSubscriber::getSubscriber("/test3/", subCb, errCb);

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