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
#include <EZMQXLogger.h>

#define TAG "EZMQXContext"

static const std::string COLLON = ":";
static const std::string SLASH = "/";
static const std::string TNS_KNOWN_PORT = "48323";

// Rest Endpoints
static const std::string NODE = "http://172.17.0.1:48098";
static const std::string PREFIX = "/api/v1";
static const std::string API_CONFIG = "/management/device/configuration";
static const std::string API_APPS = "/management/apps";
static const std::string API_DETAIL = "/management/detail";

// JSON Keys
static const std::string CONF_PROPS = "properties";
static const std::string CONF_REMOTE_ADDR = "anchoraddress";
static const std::string CONF_NODE_ADDR = "nodeaddress";
static const std::string APPS_PROPS = "apps";
static const std::string APPS_ID = "id";
static const std::string APPS_STATE = "state";
static const std::string APPS_STATE_RUNNING = "running";
static const std::string SERVICES_PROPS = "services";
static const std::string SERVICES_CON_NAME = "name";
static const std::string SERVICES_CON_ID = "cid";
static const std::string SERVICES_CON_PORTS = "ports";
static const std::string PORTS_PRIVATE = "PrivatePort";
static const std::string PORTS_PUBLIC = "PublicPort";


// hostname path
static const std::string HOSTNAME = "/etc/hostname";

// Dynamic port info
static const int LOCAL_PORT_START = 4000;
static const int LOCAL_PORT_MAX = 100;

// ctor
EZMQX::Context::Context() : initialized(false), terminated(false), usedIdx(0), numOfPort(0), standAlone(false), tnsEnabled(false)
{
    if (ezmq::EZMQ_OK != ezmq::EZMQAPI::getInstance()->initialize())
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not start ezmq context", __func__);
        throw EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
    }
}

// dtor
EZMQX::Context::~Context()
{
    terminate();
}

void EZMQX::Context::setStandAloneMode(bool mode)
{
    EZMQX_LOG_V(INFO, TAG, "%s StandAlone mode enabled", __func__);
    this->standAlone = mode;
    if (this->standAlone)
    {
        initialized.store(true);
    }
    else
    {
        initialize();
    }
}

void EZMQX::Context::setHostInfo(std::string hostName, std::string hostAddr)
{
    EZMQX_LOG_V(INFO, TAG, "%s Host infomation setted manually Hostname: %s Hostaddr: %s", __func__, hostName, hostAddr);
    this->hostname = hostname;
    this->hostAddr = hostAddr;
}

void EZMQX::Context::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(INFO, TAG, "%s TNS addr setted manually Addr: %s", __func__, remoteAddr);
    tnsEnabled = true;
    this->remoteAddr = remoteAddr;
}

EZMQX::Context* EZMQX::Context::getInstance()
{
    static EZMQX::Context _instance;
    return &_instance;
}

EZMQX::Endpoint EZMQX::Context::getHostEp(int port)
{
    int hostPort = 0;
    if (standAlone)
    {
        hostPort = port;
    }
    else
    {
        hostPort = ports[port];
        if (hostPort == 0)
        {
            throw EZMQX::Exception("Invalid Port", EZMQX::UnKnownState);
        }
    }

    EZMQX::Endpoint ep(hostAddr, hostPort);

    EZMQX_LOG_V(DEBUG, TAG, "%s Port: %d, Host Addr: %s", __func__, port, ep.toString());

    return ep;
}

std::list<std::string> EZMQX::Context::addAmlRep(const std::list<std::string>& amlModelInfo)
{
    std::list<std::string> modelId;

    if (amlModelInfo.empty())
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s given list is empty", __func__);
        return modelId;
    }

    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        for (std::list<std::string>::const_iterator itr = amlModelInfo.cbegin(); itr != amlModelInfo.cend(); itr++)
        {
            std::string path = *itr;
            if (path.empty())
            {
                throw EZMQX::Exception("Invalid aml model path", EZMQX::UnKnownState);
            }
            else
            {
                std::string amlModelId;
                AML::Representation* repPtr = nullptr;
                try
                {
                    repPtr = new AML::Representation(path);
                }
                catch(...)
                {
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::UnKnownState);
                }

                if (!repPtr)
                {
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::UnKnownState);
                }

                std::shared_ptr<AML::Representation> rep(repPtr);

                try
                {
                    amlModelId = rep->getRepresentationId();
                }
                catch(...)
                {
                    throw EZMQX::Exception("Invalid aml model id", EZMQX::UnKnownState);
                }

                if (amlModelId.empty())
                {
                    throw EZMQX::Exception("Invalid aml model id", EZMQX::UnKnownState);
                }

                try
                {
                    // ignore duplicated rep
                    if (amlRepDic.find(amlModelId) == amlRepDic.end())
                    {
                        amlRepDic.insert(std::pair<std::string, std::shared_ptr<AML::Representation>>(amlModelId, rep));
                    }

                    modelId.push_back(amlModelId);
                }
                catch(...)
                {
                    throw EZMQX::Exception("Unknown error", EZMQX::UnKnownState);
                }
            }
        }
    }
    // mutex unlock
    return modelId;
}

std::shared_ptr<AML::Representation> EZMQX::Context::getAmlRep(const std::string& amlModelId)
{
    std::map<std::string, std::shared_ptr<AML::Representation>>::iterator itr;
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        itr = amlRepDic.find(amlModelId);

        if (itr == amlRepDic.end())
        {
            throw EZMQX::Exception("Could not matched Aml Rep", EZMQX::UnKnownState);
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

                        if (props[i].isMember(CONF_REMOTE_ADDR))
                        {
                            setTnsInfo(props[i][CONF_REMOTE_ADDR].asString() + COLLON + TNS_KNOWN_PORT);
                        }

                        if (props[i].isMember(CONF_NODE_ADDR))
                        {
                            this->hostAddr = props[i][CONF_NODE_ADDR].asString();
                        }
                    }
                }
            }
            catch(...)
            {
                throw EZMQX::Exception("Internal rest service unavilable", EZMQX::ServiceUnavailable);
            }

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
                throw EZMQX::Exception("Could not found hostname", EZMQX::UnKnownState);
            }

            // for logging
            std::cout << this->remoteAddr <<std::endl;
            std::cout << this->hostAddr <<std::endl;
            std::cout << this->hostname <<std::endl;

            // parse port mapping table
            try
            {
                nodeInfo.clear();
                // get apps info from node

                {
                    EZMQX::SimpleRest rest;
                    nodeInfo = rest.Get(NODE+PREFIX+API_APPS);
                }

            }
            catch(...)
            {
                throw EZMQX::Exception("Internal rest service unavilable", EZMQX::ServiceUnavailable);
            }

            Json::Value root;
            Json::Reader reader;
            Json::Value props;
            Json::Value portArray;
            std::list<std::string> runningApps;

            try
            {
                if (!reader.parse(nodeInfo, root))
                {
                    throw EZMQX::Exception("Could not parse json", EZMQX::UnKnownState);
                }
                else
                {
                    props = root[APPS_PROPS];

                    // apps
                    for (Json::Value::ArrayIndex i = 0; i < props.size(); i ++)
                    {
                        if (props[i].isMember(APPS_ID)
                            && props[i].isMember(APPS_STATE)
                            && props[i][APPS_STATE].asString() == APPS_STATE_RUNNING)
                        {
                            std::string appId = props[i][APPS_ID].asString();
                            if (appId.empty())
                            {
                                continue;
                            }
                            else
                            {
                                runningApps.push_back(appId);
                            }
                        }
                    }
                }
            }
            catch(...)
            {
                throw EZMQX::Exception("There is no running application", EZMQX::ServiceUnavailable);
            }

            // get app info detail
            try
            {
                bool found = false;

                for (std::list<std::string>::iterator itr = runningApps.begin(); itr != runningApps.end(); itr++)
                {
                    if (found)
                    {
                        break;
                    }

                    std::string appId = *itr;
                    nodeInfo.clear();
                    {
                        EZMQX::SimpleRest rest;
                        nodeInfo = rest.Get(NODE + PREFIX + API_APPS + SLASH + appId);
                    }

                    if (nodeInfo.empty())
                    {
                        continue;
                    }
                    else
                    {
                        root.clear();
                        if (!reader.parse(nodeInfo, root))
                        {
                            continue;
                        }
                        else
                        {
                            props.clear();
                            props = root[SERVICES_PROPS];
                            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
                            {
                                if (props[i].isMember(SERVICES_CON_ID) && props[i].isMember(SERVICES_CON_PORTS))
                                {
                                    std::string conId = props[i][SERVICES_CON_ID].asString();
                                    conId = conId.substr(0, hostname.size());
                                    if (!conId.compare(hostname))
                                    {
                                        portArray = props[i][SERVICES_CON_PORTS];
                                        found = true;
                                        break;
                                    }
                                    else
                                    {
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch(...)
            {
                throw EZMQX::Exception("Could not parse app detail info", EZMQX::UnKnownState);
            }

            try
            {
                for (Json::Value::ArrayIndex i = 0; i < portArray.size(); i ++)
                {
                    if (portArray[i].isMember(PORTS_PRIVATE) && portArray[i].isMember(PORTS_PUBLIC))
                    {

                        int privatePort = std::stoi(portArray[i][PORTS_PRIVATE].asString());
                        int publicPort = std::stoi(portArray[i][PORTS_PUBLIC].asString());
                        if (privatePort > -1 && publicPort > -1)
                        {
                            ports[privatePort] = publicPort;
                        }
                    }
                }
            }
            catch(...)
            {
                throw EZMQX::Exception("Could not found port mapping info", EZMQX::UnKnownState);
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

bool EZMQX::Context::isStandAlone()
{
    return standAlone;
}

bool EZMQX::Context::isTnsEnabled()
{
    return tnsEnabled;
}

std::string EZMQX::Context::getTnsAddr()
{
    return remoteAddr;
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
            if (!standAlone)
            {
                    EZMQX::SimpleRest rest;
                    // tns rest api not provide yet
            }

            // release resource
            ports.clear();
            ezmq::EZMQAPI::getInstance()->terminate();
        }
        else
        {
            throw EZMQX::Exception("Context terminated", EZMQX::Terminated);
        }

        terminated.store(true);
    }
    // mutex unlock
    
    return;
}

void EZMQX::Context::insertTopic(std::string topic)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        topicList.push_back(topic);
    }
    // mutex unlock
}

void EZMQX::Context::deleteTopic(std::string topic)
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        auto itr = std::find(topicList.begin(), topicList.end(), topic);

        if (itr != topicList.end())
        {
            topicList.erase(itr);
        }
    }
    // mutex unlock
}

std::list<std::string> EZMQX::Context::getTopicList()
{
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        return topicList;
    }
    // mutex unlock
    return topicList;
}