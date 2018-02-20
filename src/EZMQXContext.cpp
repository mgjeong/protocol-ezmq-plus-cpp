#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <curl.h>

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
            CURL *curl;
            CURLcode res;
            curl_global_init(CURL_GLOBAL_DEFAULT);
            curl = curl_easy_init();

            // parse host & broker addr

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
        else
        {
            throw new EZMQX::Exception("Context terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    
    return;
}