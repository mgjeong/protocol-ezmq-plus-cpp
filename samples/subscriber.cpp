#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <EZMQXSubscriber.h>
// #include <EZMQXTopic.h>
// #include <EZMQXEndpoint.h>
#include <signal.h>
// #include <condition_variable>
// #include <memory>

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
      std::shared_ptr<EZMQX::Config> config = EZMQX::Config::getInstance();
      amlId = config->addAmlModel(amlPath);

      // error callback
      // typedef std::function<void(std::string topic, const AMLObject& payload)> SubCb;
      // typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;
      EZMQX::SubCb subCb = [](std::string topic, const AMLObject& payload){std::cout << "subCb called" << std::endl;};
      EZMQX::SubErrCb errCb = [](std::string topic, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl;};
      
      // create subscriber with test topic
      EZMQX::Endpoint ep("localhost", 4000);

      EZMQX::Topic topic("test", amlId.front(), ep);
      std::shared_ptr<EZMQX::Subscriber> subscriber = EZMQX::Subscriber::getSubscriber(topic, subCb, errCb);

      // push to blocked
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