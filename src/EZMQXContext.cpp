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

// Rest Endpoints
static const std::string NODE = "http://172.17.0.1:48098";
static const std::string PREFIX = "/api/v1";
static const std::string API_CONFIG = "/management/device/configuration";
static const std::string API_APPS = "/management/apps";
static const std::string API_DETAIL = "/management/detail";

// JSON Keys
static const std::string CONF_PROPS = "properties";
static const std::string CONF_NAME = "name";
static const std::string CONF_VALUE = "value";
static const std::string CONF_REMOTE_ADDR = "anchoraddress";
static const std::string CONF_NODE_ADDR = "nodeaddress";

// hostname path
static const std::string HOSTNAME = "/etc/hostname";

std::shared_ptr<EZMQX::Context> EZMQX::Context::_instance;

// ctor
EZMQX::Context::Context() : initialized(false), terminated(false)
{
    initialize();
}

// ctor for fake object that used in unittest
EZMQX::Context::Context(std::string fakeHostname, std::string fakeHostAddr, std::string fakeRemoteAddr, std::map<int, int> fakePorts) : initialized(true), terminated(false)
{
    this->hostname = fakeHostname;
    this->hostAddr = fakeHostAddr;
    this->remoteAddr = fakeRemoteAddr;
    this->ports = fakePorts;
}

// dtor
EZMQX::Context::~Context()
{
    terminate();
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
    int hostPort = ports[port];
    if (hostPort == 0)
    {
        throw new EZMQX::Exception("Invalid Port", EZMQX::UnKnownState);
    }

    EZMQX::Endpoint ep(hostAddr, hostPort);
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

            try
            {
                {
                    EZMQX::SimpleRest rest;
                    nodeInfo = rest.Get(NODE+PREFIX+API_CONFIG);
                }

                Json::Value root;
                Json::Reader reader;
                if (!reader.parse(nodeInfo, root))
                {
                    std::cout << "json parse fail" << std::endl;
                }
                else
                {
                    Json::Value props = root[CONF_PROPS];

                    for (Json::Value::ArrayIndex i = 0; i < props.size(); i ++)
                    {
                        if (!this->remoteAddr.empty() && !this->hostAddr.empty())
                        {
                            break;
                        }

                        if (props[i].isMember(CONF_NAME))
                        {

                             if (props[i][CONF_NAME].asString() == CONF_REMOTE_ADDR)
                             {
                                this->remoteAddr = props[i][CONF_VALUE].asString();

                             }
                             else if (props[i][CONF_NAME].asString() == CONF_NODE_ADDR )
                             {
                                this->hostAddr = props[i][CONF_VALUE].asString();
                             }
                        }
                    }
                }
            }
            catch(...)
            {
                throw new EZMQX::Exception("Internal rest service unavilable", EZMQX::ServiceUnavailable);
            }

            // std::cout << nodeInfo <<std::endl;

            if (nodeInfo.empty())
            {
                // do something
            }

            try
            {
                // get hostname
                std::ifstream _file(HOSTNAME);
                std::string _hostname((std::istreambuf_iterator<char>(_file)), std::istreambuf_iterator<char>());
                // check last is '\n'
                if (_hostname.back() == '\n')
                {
                    _hostname = _hostname.substr(0, _hostname.size()-1);
                }
                this->hostname = _hostname;
            }
            catch(...)
            {
                throw new EZMQX::Exception("Could not found hostname", EZMQX::UnKnownState);
            }

            // for logging
            std::cout << this->remoteAddr <<std::endl;
            std::cout << this->hostAddr <<std::endl;
            std::cout << this->hostname <<std::endl;

            // parse port mapping table
            try
            {
                int count = 0;
                nodeInfo.clear();
                // get apps info from node
                {
                    EZMQX::SimpleRest rest;
                    // rest api will change
                    // nodeInfo = rest.Get(NODE+PREFIX+API_APPS);
                    // get count

                }

                for (int i = 0; i< count; i++)
                {
                    // get app detail info
                    // rest api not provide yet
                }
            }
            catch(...)
            {
                throw new EZMQX::Exception("Internal rest service unavilable", EZMQX::ServiceUnavailable);
            }

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
            // send dead msg to tns server
            {
                    EZMQX::SimpleRest rest;
                    // tns rest api not provide yet
            }

            // release resource
            ports.clear();
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