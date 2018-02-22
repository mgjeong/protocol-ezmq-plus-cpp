#include <EZMQXContext.h>
#include <EZMQXErrorCode.h>
#include <EZMQXException.h>
#include <EZMQXRest.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

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

            // parse host & broker addr
            std::string nodeInfo;
            {
                EZMQX::SimpleRest rest;
                nodeInfo = rest.Get("http://10.113.77.245:48098/api/v1/management/device/configuration");
            }

            // std::cout << nodeInfo <<std::endl;

            if (nodeInfo.empty())
            {
                // do something
            }

            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(nodeInfo, root))
            {
                std::cout << "json parse fail" << std::endl;
            }
            else
            {
                Json::Value props = root["properties"];

                for (Json::Value::ArrayIndex i = 0; i < props.size(); i ++)
                {
                    if (!this->remoteAddr.empty() && !this->hostAddr.empty())
                    {
                        break;
                    }

                    if (props[i].isMember("name"))
                    {

                         if (props[i]["name"].asString() == "anchoraddress")
                         {
                            this->remoteAddr = props[i]["value"].asString();

                         }
                         else if (props[i]["name"].asString() == "nodeaddress")
                         {
                            this->hostAddr = props[i]["value"].asString();
                         }
                    }
                }
            }


            // get hostname
            std::ifstream _file("/etc/hostname");
            std::string _hostname((std::istreambuf_iterator<char>(_file)), std::istreambuf_iterator<char>());
            // check last is '\n'
            if (_hostname.back() == '\n')
            {
                _hostname = _hostname.substr(0, _hostname.size()-1);
            }
            this->hostname = _hostname;


            std::cout << this->remoteAddr <<std::endl;
            std::cout << this->hostAddr <<std::endl;
            std::cout << this->hostname <<std::endl;

            // parse port mapping table

            // get apps info from node
                // get app detail info
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