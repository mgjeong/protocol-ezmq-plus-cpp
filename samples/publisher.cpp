#include <iostream>
#include <EZMQXPublisher.h>
#include <EZMQXException.h>
int main()
{

    int dummy = 3;
    void *payload = (void *)&dummy;
    try
    {
       std::shared_ptr<EZMQX::Publisher> publisher = EZMQX::Publisher::getPublisher("/topic");
        
       publisher->publish(payload);

       if (publisher->isTerminated())
       {
            publisher->terminate();
       }
    }
    catch(EZMQX::Exception e)
    {
        std::cout << e.what() << std::endl;
    }

    std::cout << "done" << std::endl;
}