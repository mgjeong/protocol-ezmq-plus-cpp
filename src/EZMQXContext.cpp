#include <EZMQXContext.h>

std::shared_ptr<EZMQX::Context> EZMQX::Context::_instance;

// ctor
EZMQX::Context::Context() : initialized(false), terminated(false)
{
    initialize();
}

std::shared_ptr<EZMQX::Context> EZMQX::Context::getInstance()
{
    if (!_instance)
    {
        _instance.reset(new EZMQX::Context());
    }
    

    return _instance;
}

EZMQX::Endpoint EZMQX::Context::getHostEp(int port)
{
    EZMQX::Endpoint ep("", -1);
    return ep;
}

void EZMQX::Context::initialize()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!initialized.load())
        {

            // initialize resource

            // parse host addr

            // parse broker addr

            // get hostname

            // parse port mapping table
        }

        initialized.store(true);
    }
    // mutex unlock
    return;
}

bool EZMQX::Context::isInitialized()
{
    //atomically
    return initialized.load();
}

bool EZMQX::Context::isTerminated()
{
    //atomically
    return terminated.load();
}

void EZMQX::Context::terminate()
{   
    // throw exception
        // NotInitialized

    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!terminated.load())
        {
            // release resource
        }

        terminated.store(true);
    }
    // mutex unlock
    
    return;
}