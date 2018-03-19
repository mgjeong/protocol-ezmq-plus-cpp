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

// Dynamic port info
static const int LOCAL_PORT_START = 4000;
static const int LOCAL_PORT_MAX = 100;

std::shared_ptr<EZMQX::Context> EZMQX::Context::_instance;

// ctor
EZMQX::Context::Context() : initialized(false), terminated(false), usedIdx(0), numOfPort(0)
{
    ezmqCtx.reset(ezmq::EZMQAPI::getInstance());
    if (ezmq::EZMQ_OK != ezmqCtx->initialize())
    {
        throw new EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
    }
    //initialize();
}

// ctor for fake object that used in unittest
EZMQX::Context::Context(std::string fakeHostname, std::string fakeHostAddr, std::string fakeRemoteAddr, std::map<int, int> fakePorts) : initialized(true), terminated(false), usedIdx(0)
{
    ezmqCtx.reset(ezmq::EZMQAPI::getInstance());
    if (ezmq::EZMQ_OK != ezmqCtx->initialize())
    {
        throw new EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
    }

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

std::list<std::string> EZMQX::Context::addAmlRep(const std::list<std::string>& amlModelInfo)
{
    std::list<std::string> modelId;
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (amlModelInfo.empty())
        {
            // throw exceptionn
        }
        else
        {
            for (std::list<std::string>::const_iterator itr = amlModelInfo.cbegin(); itr != amlModelInfo.cend(); itr++)
            {
                std::string path = *itr;
                if (path.empty())
                {
                    // throw exception
                }
                else
                {
                    try
                    {
                        std::shared_ptr<Representation> rep = std::make_shared<Representation>(path);
                        std::string amlModelId = rep->getRepresentationId();
                        if (amlModelId.empty())
                        {
                            // throw invalid aml model exception
                        }
                        else
                        {
                            // ignore duplicated rep
                            if (amlRepDic.find(amlModelId) == amlRepDic.end())
                            {
                                amlRepDic.insert(std::pair<std::string, std::shared_ptr<Representation>>(amlModelId, rep));
                            }

                            modelId.insert(modelId.end(), amlModelId);
                        }
                    }
                    catch(...)
                    {
                        // throw invalid aml file path exception
                        // throw invalid aml model exception
                    }
                }
            }
        }
    }
    // mutex unlock
    return modelId;
}

std::shared_ptr<Representation> EZMQX::Context::getAmlRep(const std::string& amlModelId)
{
    std::map<std::string, std::shared_ptr<Representation>>::iterator itr;
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        itr = amlRepDic.find(amlModelId);

        if (itr == amlRepDic.end())
        {
            throw new EZMQX::Exception("Could not matched Aml Rep", EZMQX::UnKnownState);
        }
        else
        {
            return itr->second;
        }
    }
    // mutex unlock
}

int EZMQX::Context::assignDynamicPort()
{
    int ret = 0;
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        //get Random port
        while (1)
        {
            if (numOfPort >= LOCAL_PORT_MAX)
            {
                // throw maximum port exceed exception
            }

            if (usedPorts[LOCAL_PORT_START + usedIdx] == true)
            {
                usedIdx++;
                if ( usedIdx > LOCAL_PORT_MAX)
                {
                    usedIdx = 0;
                }
            }
            else
            {
                // mark
                usedPorts[LOCAL_PORT_START + usedIdx] = true;
                ret = LOCAL_PORT_START + usedIdx;
                numOfPort++;
                break;
            }
        }

    }
    // mutex unlock
    return ret;
}

void EZMQX::Context::releaseDynamicPort(int port)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (usedPorts[port] == true)
        {
            usedPorts[port] = false;
            numOfPort--;
        }
        else
        {
            //throw wrong port exception
        }
    }
    // mutex unlock
    return;
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

            initialized.store(true);
        }
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