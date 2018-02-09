#include <iostream>
#include <EZMQXPublisher.h>
#include <EZMQXException.h>
int main()
{

    int dummy = 3;
    void *payload = (void *)&dummy;
    try
    {
      // error callback
      EZMQX::PubErrCb errCb = [](std::shared_ptr<EZMQX::Publisher> publisher, EZMQX::ErrorCode errCode){std::cout << "called" << std::endl;};
      
      // create publisher with test topic
      std::shared_ptr<EZMQX::Publisher> publisher = EZMQX::Publisher::getPublisher("/topic", "test topic", errCb);
      
      // publish
      publisher->publish(payload);

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
