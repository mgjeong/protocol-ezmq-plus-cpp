#include <iostream>
#include <EZMQXSubscriber.h>
#include <EZMQXException.h>
int main()
{

    try
    {
      // error callback
      EZMQX::SubCb subCb = [](std::string topic, void* object){std::cout << "subCb called" << std::endl;};
      EZMQX::SubErrCb errCb = [](std::shared_ptr<EZMQX::Subscriber> subscriber, EZMQX::ErrorCode errCode){std::cout << "errCb called" << std::endl;};
      
      
      // create subscriber with test topic
      std::shared_ptr<EZMQX::Subscriber> subscriber = EZMQX::Subscriber::getSubscriber("/topic", subCb, errCb);

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