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
#include <json/writer.h>
#include <EZMQXLogger.h>
#include <EZMQXKeepAlive.h>

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

static const std::string PAYLOAD_TOPIC = "topic";
static const std::string PAYLOAD_NAME = "name";
static const std::string PAYLOAD_ENDPOINT = "endpoint";
static const std::string PAYLOAD_DATAMODEL = "datamodel";

// hostname path
static const std::string HOSTNAME = "/etc/hostname";

// Dynamic port info
static const int LOCAL_PORT_START = 4000;
static const int LOCAL_PORT_MAX = 100;

// ctor
EZMQX::Context::Context() : keepAlive(nullptr), initialized(false), terminated(false), usedIdx(0), numOfPort(0), standAlone(false), tnsEnabled(false)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    if (ezmq::EZMQ_OK != ezmq::EZMQAPI::getInstance()->initialize())
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not start ezmq context", __func__);
        throw EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
    }
}

// dtor
EZMQX::Context::~Context()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Context::setStandAloneMode(bool mode)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s StandAlone mode enabled", __func__);
    this->standAlone = mode;
    if (this->standAlone)
    {
        initialized.store(true);
        terminated.store(false);
    }
    else
    {
        initialize();
    }
}

void EZMQX::Context::setHostInfo(std::string hostName, std::string hostAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s Host infomation setted manually Hostname: %s Hostaddr: %s", __func__, hostName.c_str(), hostAddr.c_str());
    this->hostname = hostname;
    this->hostAddr = hostAddr;
}

void EZMQX::Context::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s TNS addr setted manually Addr: %s", __func__, remoteAddr.c_str());
    tnsEnabled = true;
    this->remoteAddr = remoteAddr;
    keepAlive = new EZMQX::KeepAlive(this->remoteAddr);
}

EZMQX::Context* EZMQX::Context::getInstance()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    static EZMQX::Context _instance;
    return &_instance;
}

EZMQX::Endpoint EZMQX::Context::getHostEp(int port)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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

    EZMQX_LOG_V(DEBUG, TAG, "%s Port: %d, Host Addr: %s", __func__, port, ep.toString().c_str());

    return ep;
}

std::list<std::string> EZMQX::Context::addAmlRep(const std::list<std::string>& amlModelInfo)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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
                EZMQX_LOG_V(ERROR, TAG, "%s Invalid aml model path %s", __func__, path.c_str());
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
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse aml model file: %s", __func__, path.c_str());
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::UnKnownState);
                }

                if (!repPtr)
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse aml model file: %s", __func__, path.c_str());
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::UnKnownState);
                }

                std::shared_ptr<AML::Representation> rep(repPtr);

                try
                {
                    amlModelId = rep->getRepresentationId();
                }
                catch(...)
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Invalid aml model id", __func__);
                    throw EZMQX::Exception("Invalid aml model id", EZMQX::UnKnownState);
                }

                if (amlModelId.empty())
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Invalid aml model id", __func__);
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
                    EZMQX_LOG_V(ERROR, TAG, "%s Unknown error", __func__);
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
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    std::map<std::string, std::shared_ptr<AML::Representation>>::iterator itr;
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        itr = amlRepDic.find(amlModelId);

        if (itr == amlRepDic.end())
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not find matched Aml Rep: %s", __func__, amlModelId.c_str());
            throw EZMQX::Exception("Could not find matched Aml Rep", EZMQX::UnKnownState);
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
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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
                EZMQX_LOG_V(ERROR, TAG, "%s Could not assign port", __func__);
                throw EZMQX::Exception("Could not assign port", EZMQX::MaximumPortExceed);
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

    EZMQX_LOG_V(DEBUG, TAG, "%s port %d assigned", __func__, ret);
    // mutex unlock
    return ret;
}

void EZMQX::Context::releaseDynamicPort(int port)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        if (usedPorts[port] == true)
        {
            usedPorts[port] = false;
            numOfPort--;
            EZMQX_LOG_V(DEBUG, TAG, "%s port %d release successfully", __func__, port);
        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not release port %d", __func__, port);
            throw EZMQX::Exception("Could not release port", EZMQX::ReleaseWrongPort);
        }
    }
    // mutex unlock
    return;
}

void EZMQX::Context::initialize()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
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
                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request for getting host info", __func__);
                {
                    nodeInfo = EZMQX::RestService::Get(NODE+PREFIX+API_CONFIG).getPayload();
                }
                EZMQX_LOG_V(DEBUG, TAG, "%s Rest result \n %s \n", __func__, nodeInfo.c_str());

                Json::Value root;
                Json::Reader reader;
                if (!reader.parse(nodeInfo, root))
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
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
                            EZMQX_LOG_V(DEBUG, TAG, "%s TNS info found %s", __func__, (props[i][CONF_REMOTE_ADDR].asString()).c_str());
                            setTnsInfo(props[i][CONF_REMOTE_ADDR].asString());
                        }

                        if (props[i].isMember(CONF_NODE_ADDR))
                        {
                            EZMQX_LOG_V(DEBUG, TAG, "%s Host info found %s", __func__, props[i][CONF_NODE_ADDR].asString().c_str());
                            this->hostAddr = props[i][CONF_NODE_ADDR].asString();
                        }
                    }
                }
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Internal rest service unavilable", __func__);
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
                EZMQX_LOG_V(DEBUG, TAG, "%s hostname found %s", __func__, this->hostname);
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not found hostname", __func__);
                throw EZMQX::Exception("Could not found hostname", EZMQX::UnKnownState);
            }

            // parse port mapping table
            try
            {
                nodeInfo.clear();
                // get apps info from node
                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request for getting App info", __func__);

                {
                    nodeInfo = EZMQX::RestService::Get(NODE+PREFIX+API_APPS).getPayload();
                }

            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Internal rest service unavilable", __func__);
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
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
                    throw EZMQX::Exception("Could not parse json", EZMQX::UnKnownState);
                }
                else
                {
                    props = root[APPS_PROPS];

                    // apps
                    EZMQX_LOG_V(DEBUG, TAG, "%s %d App information found", __func__, props.size());
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
                                EZMQX_LOG_V(DEBUG, TAG, "%s Found running application ID: %s", __func__, appId.c_str());
                                runningApps.push_back(appId);
                            }
                        }
                    }
                }
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s There is no running application", __func__);
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
                    std::string addr = NODE + PREFIX + API_APPS + SLASH + appId;
                    nodeInfo.clear();
                    EZMQX_LOG_V(DEBUG, TAG, "%s try send Get request to %s", __func__, addr.c_str());
                    {
                        nodeInfo = EZMQX::RestService::Get(addr).getPayload();
                    }
                    EZMQX_LOG_V(DEBUG, TAG, "%s Rest result is \n %s \n", __func__, nodeInfo.c_str());

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
                                        EZMQX_LOG_V(DEBUG, TAG, "%s Application found!!! %s ", __func__, conId.c_str());
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
                EZMQX_LOG_V(ERROR, TAG, "%s Could not parse app detail info", __func__);
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
                            EZMQX_LOG_V(DEBUG, TAG, "%s parsed port info Private: %d | Public: %d ", __func__, privatePort, publicPort);
                            ports[privatePort] = publicPort;
                        }
                    }
                }
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not found port mapping info", __func__);
                throw EZMQX::Exception("Could not found port mapping info", EZMQX::UnKnownState);
            }

            initialized.store(true);
            terminated.store(false);
        }
    }
    // mutex unlock
    return;
}

bool EZMQX::Context::isInitialized()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    //atomically
    return initialized.load();
}

bool EZMQX::Context::isTerminated()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    //atomically
    return terminated.load();
}

bool EZMQX::Context::isStandAlone()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return standAlone;
}

bool EZMQX::Context::isTnsEnabled()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return tnsEnabled;
}

std::string EZMQX::Context::getTnsAddr()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return remoteAddr;
}

void EZMQX::Context::terminate()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // throw exception
        // NotInitialized
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (!terminated.load())
        {
            if (keepAlive)
            {
                if (topicList.empty())
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s topic list is empty try delete KeepAlive", __func__);
                    delete keepAlive;
                }
                else
                {
                    // throw exception
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not terminate context threre are active topic", __func__);
                    throw EZMQX::Exception("Could not terminate context threre are active topic", EZMQX::UnKnownState);
                }
            }

            // release resource
            EZMQX_LOG_V(DEBUG, TAG, "%s try release resources", __func__);
            ports.clear();
            usedPorts.clear();
            amlRepDic.clear();
            hostname.clear();
            hostAddr.clear();
            remoteAddr.clear();
            usedIdx = 0;
            numOfPort = 0;
            standAlone = false;
            tnsEnabled = false;

            EZMQX_LOG_V(DEBUG, TAG, "%s try release EZMQAPI", __func__);
            ezmq::EZMQAPI::getInstance()->terminate();
        }
        else
        {
            EZMQX_LOG_V(INFO, TAG, "%s Context already terminated", __func__);
        }

        terminated.store(true);
        initialized.store(false);
    }
    // mutex unlock
    
    return;
}

void EZMQX::Context::insertTopic(std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        topicList.push_back(topic);
        EZMQX_LOG_V(DEBUG, TAG, "%s topic inserted %s", __func__, topic.c_str());
    }
    // mutex unlock
}

void EZMQX::Context::deleteTopic(std::string topic)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        auto itr = std::find(topicList.begin(), topicList.end(), topic);

        if (itr != topicList.end())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s topic %s deleted from list", __func__, topic.c_str());
            topicList.erase(itr);

            if (keepAlive)
            {
                try
                {
                    EZMQX_LOG_V(DEBUG, TAG, "%s Request unregister topic %s", __func__, topic.c_str());
                    keepAlive->inQue(EZMQX::UnregisterTopic, topic);
                }
                catch(...)
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not request UnregisterTopic", __func__);
                }
            }
        }
    }
    // mutex unlock
}

std::list<std::string> EZMQX::Context::getTopicList()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        std::lock_guard<std::mutex> scopedLock(lock);
        return topicList;
    }
    // mutex unlock
    return topicList;
}