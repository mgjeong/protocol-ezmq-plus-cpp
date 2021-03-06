/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/

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
#include <EZMQXPublisher.h>
#include <EZMQXSubscriber.h>
#include <algorithm>

#define TAG "EZMQXContext"
static const std::string COLLON = ":";
static const std::string SLASH = "/";
static const std::string TNS_KNOWN_PORT = "48323";
static const std::string REVERSE_PROXY_KNOWN_PORT = "80";

// Rest Endpoints
static const std::string HTTP = "http://";
static const std::string NODE = "http://pharos-node:48098";
static const std::string PREFIX = "/api/v1";
static const std::string API_CONFIG = "/management/device/configuration";
static const std::string API_APPS = "/management/apps";
static const std::string API_DETAIL = "/management/detail";
static const std::string API_SEARCH_NODE = "/search/nodes";
static const std::string REVERSE_PROXY_PREFIX = "/tns-server";


// REST query options
static const std::string ANCHOR_IMAGE_NAME = "imageName=";

// JSON Keys
static const std::string IMG_NAME = "imageName";
static const std::string CONF_PROPS = "properties";
static const std::string CONF_ANCHOR_ADDR = "anchorendpoint";
static const std::string CONF_NODE_ADDR = "nodeaddress";
static const std::string CONF_REVERSE_PROXY = "reverseproxy";
static const std::string CONF_REVERSE_PROXY_ENABLED = "enabled";
static const std::string NODES = "nodes";
static const std::string NODES_STATUS = "status";
static const std::string NODES_CONNECTED = "connected";
static const std::string NODES_IP = "ip";
static const std::string NODES_CONF = "config";
static const std::string NODES_PROPS = "properties";
static const std::string NODES_REVERSE_PROXY = "reverseproxy";
static const std::string NODES_REVERSE_PROXY_ENABLED = "enabled";
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
EZMQX::Context::Context() : keepAlive(nullptr), standAlone(false), tnsEnabled(false), initialized(false), terminated(false), reverseProxy(false), interval(-1), usedIdx(0), numOfPort(0)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
}

// dtor
EZMQX::Context::~Context()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    terminate();
}

void EZMQX::Context::setStandAloneMode(bool mode, const std::string& tnsConfPathRef)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s StandAlone mode enabled", __func__);
    this->standAlone = mode;
    if (this->standAlone)
    {
        if (ezmq::EZMQ_OK != ezmq::EZMQAPI::getInstance()->initialize())
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not start ezmq context", __func__);
            throw EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
        }
        
        initialized.store(true);
        terminated.store(false);
    }
    else
    {
        initialize(tnsConfPathRef);
    }
}

void EZMQX::Context::setHostInfo(std::string hostName, std::string hostAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s Host infomation setted manually Hostname: %s Hostaddr: %s", __func__, hostName.c_str(), hostAddr.c_str());
    this->hostname = hostName;
    this->hostAddr = hostAddr;
}

void EZMQX::Context::setTnsInfo(std::string remoteAddr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    EZMQX_LOG_V(INFO, TAG, "%s TNS addr setted manually Addr: %s", __func__, remoteAddr.c_str());
    tnsEnabled = true;
    this->tnsAddr = remoteAddr;
    
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
                throw EZMQX::Exception("Invalid aml model path", EZMQX::InvalidParam);
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
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::InvalidAmlModel);
                }

                if (!repPtr)
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse aml model file: %s", __func__, path.c_str());
                    throw EZMQX::Exception("Could not parse aml model file : " + path, EZMQX::InvalidAmlModel);
                }

                std::shared_ptr<AML::Representation> rep(repPtr);

                try
                {
                    amlModelId = rep->getRepresentationId();
                }
                catch(...)
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Invalid aml model id", __func__);
                    throw EZMQX::Exception("Invalid aml model id", EZMQX::InvalidParam);
                }

                if (amlModelId.empty())
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Invalid aml model id", __func__);
                    throw EZMQX::Exception("Invalid aml model id", EZMQX::InvalidParam);
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
            throw EZMQX::Exception("Could not find matched Aml Rep", EZMQX::UnknownAmlModel);
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

void EZMQX::Context::initialize(const std::string& tnsConfPathRef)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    // mutex lock
    {
        if (ezmq::EZMQ_OK != ezmq::EZMQAPI::getInstance()->initialize())
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not start ezmq context", __func__);
            throw EZMQX::Exception("Could not start ezmq context", EZMQX::UnKnownState);
        }

        std::lock_guard<std::mutex> scopedLock(lock);
        if (!initialized.load())
        {

            // initialize resource

            // parse host & broker addr
            std::string nodeInfo;
            getImageName(tnsConfPathRef);

            try
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request for getting host info", __func__);
                {
                    nodeInfo = EZMQX::RestService::Get(NODE+PREFIX+API_CONFIG).getPayload();
                }
                EZMQX_LOG_V(DEBUG, TAG, "%s Rest result \n %s \n", __func__, nodeInfo.c_str());

                std::string errors;
                Json::Value root;
                Json::CharReaderBuilder builder;
                std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

                if (!reader->parse(nodeInfo.c_str(), nodeInfo.c_str() + nodeInfo.size(), &root, &errors))
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
                }
                else
                {
                    Json::Value props = root[CONF_PROPS];
                    for (Json::Value::ArrayIndex i = 0; i < props.size(); i ++)
                    {
                        if (!this->anchorAddr.empty() && !this->hostAddr.empty())
                        {
                            break;
                        }

                        if (props[i].isMember(CONF_ANCHOR_ADDR))
                        {
                            EZMQX_LOG_V(DEBUG, TAG, "%s ANCHOR info found %s", __func__, (props[i][CONF_ANCHOR_ADDR].asString()).c_str());
                            this->anchorAddr = props[i][CONF_ANCHOR_ADDR].asString();
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

            // parse tns info
            try
            {
                std::string nodesInfo;
                // query to anchor
                // parse result
                // store it

                EZMQX_LOG_V(DEBUG, TAG, "%s Try send rest request to Anchor for get tns info", __func__);
                {
                    nodesInfo = EZMQX::RestService::Get(this->anchorAddr + API_SEARCH_NODE, ANCHOR_IMAGE_NAME + this->imageName).getPayload();
                }
                EZMQX_LOG_V(DEBUG, TAG, "%s Rest result \n %s \n", __func__, nodesInfo.c_str());

                std::string errors;
                Json::Value root;
                Json::CharReaderBuilder builder;
                std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

                if (!reader->parse(nodesInfo.c_str(), nodesInfo.c_str() + nodesInfo.size(), &root, &errors))
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
                }
                else
                {
                    Json::Value array = root[NODES];
                    int count = 0;
                    int marked = 0;
                    bool reverseProxyEnabled = false;
                    for (Json::Value::ArrayIndex i = 0; i < array.size(); i ++)
                    {
                        if (NODES_CONNECTED.compare(array[i][NODES_STATUS].asString()) == 0)
                        {
                            count++;
                            marked = i;
                        }
                    }

                    if (count > 1)
                    {
                        EZMQX_LOG_V(ERROR, TAG, "%s Multiple tns server found", __func__);
                        throw EZMQX::Exception("Multiple tns server found", EZMQX::ServiceUnavailable);
                    }
                    else if (count == 0)
                    {
                        EZMQX_LOG_V(ERROR, TAG, "%s Could not find tns server", __func__);
                        throw EZMQX::Exception("Could not find tns server", EZMQX::ServiceUnavailable);
                    }

                    std::string ipaddr = array[marked][NODES_IP].asString();

                    for (Json::Value::ArrayIndex i = 0; i < (array[marked][NODES_CONF][NODES_PROPS]).size(); i ++)
                    {
                        if (array[marked][NODES_CONF][NODES_PROPS][i].isMember(NODES_REVERSE_PROXY))
                        {
                            reverseProxyEnabled = array[marked][NODES_CONF][NODES_PROPS][i][NODES_REVERSE_PROXY][NODES_REVERSE_PROXY_ENABLED].asBool();
                            EZMQX_LOG_V(DEBUG, TAG, "%s Found reverse proxy option!", __func__);
                            break;
                        }
                    }

                    if (reverseProxyEnabled)
                    {
                        tnsAddr = HTTP + ipaddr + COLLON + REVERSE_PROXY_KNOWN_PORT + REVERSE_PROXY_PREFIX;
                    }
                    else
                    {
                        tnsAddr = HTTP + ipaddr + COLLON + TNS_KNOWN_PORT;
                    }

                    EZMQX_LOG_V(DEBUG, TAG, "%s tns server address is %s", __func__, this->tnsAddr.c_str());

                    this->tnsEnabled = true;

                    reverseProxy.store(reverseProxyEnabled);
                }
            }
            catch(const EZMQX::Exception& e)
            {
                throw e;
            }
            catch(...)
            {
                throw EZMQX::Exception("Could not find tns server", EZMQX::ServiceUnavailable);
            }

            try
            {
                // get hostname
                if (hostname.empty())
                {
                    std::ifstream _file(HOSTNAME);
                    std::string _hostname((std::istreambuf_iterator<char>(_file)), std::istreambuf_iterator<char>());
                    // check last is '\n'
                    if (_hostname.back() == '\n')
                    {
                        _hostname = _hostname.substr(0, _hostname.size()-1);
                    }
                    this->hostname = _hostname;
                }

                if (hostname.empty())
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse hostname", __func__);
                    throw EZMQX::Exception("Could not parse hostname", EZMQX::ServiceUnavailable);
                }

                EZMQX_LOG_V(DEBUG, TAG, "%s hostname found %s", __func__, this->hostname.c_str());
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not found hostname", __func__);
                throw EZMQX::Exception("Could not found hostname", EZMQX::ServiceUnavailable);
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

            std::string errors;
            Json::Value root;
            Json::CharReaderBuilder builder;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            Json::Value props;
            Json::Value portArray;
            std::list<std::string> runningApps;

            try
            {
                if (!reader->parse(nodeInfo.c_str(), nodeInfo.c_str() + nodeInfo.size(), &root, &errors))
                {
                    EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
                    throw EZMQX::Exception("Could not parse json", EZMQX::ServiceUnavailable);
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
            catch(const EZMQX::Exception& e)
            {
                throw e;
            }
            catch(...)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
                throw EZMQX::Exception("Could not parse json", EZMQX::ServiceUnavailable);
            }

            if (runningApps.empty())
            {
                EZMQX_LOG_V(ERROR, TAG, "%s There is no running application", __func__);
                throw EZMQX::Exception("There is no running application", EZMQX::ServiceUnavailable);
            }

            // get app info detail
            bool found = false;
            try
            {
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
                        Json::Value tmp;
                        if (!reader->parse(nodeInfo.c_str(), nodeInfo.c_str() + nodeInfo.size(), &tmp, &errors))
                        {
                            EZMQX_LOG_V(ERROR, TAG, "%s failed at parse json", __func__);
                            continue;
                        }
                        else
                        {
                            props.clear();
                            props = tmp[SERVICES_PROPS];
                            for (Json::Value::ArrayIndex i = 0; i < props.size(); i++)
                            {
                                EZMQX_LOG_V(DEBUG, TAG, "%s seeking json object", __func__);
                                if (props[i].isMember(SERVICES_CON_ID) && props[i].isMember(SERVICES_CON_PORTS))
                                {
                                    EZMQX_LOG_V(DEBUG, TAG, "%s this json object have cid, ports member", __func__);
                                    std::string conId = props[i][SERVICES_CON_ID].asString();
                                    conId = conId.substr(0, hostname.size());
                                    EZMQX_LOG_V(DEBUG, TAG, "%s cid %s, host %s", __func__, conId.c_str(), hostname.c_str());
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
                throw EZMQX::Exception("Could not parse app detail info", EZMQX::ServiceUnavailable);
            }

            if(!found)
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not parse app detail info", __func__);
                throw EZMQX::Exception("Could not parse app detail info", EZMQX::ServiceUnavailable);
            }

            try
            {
                ports.clear();
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
                throw EZMQX::Exception("Could not found port mapping info", EZMQX::ServiceUnavailable);
            }

            if (ports.empty())
            {
                EZMQX_LOG_V(ERROR, TAG, "%s Could not found port mapping info", __func__);
                throw EZMQX::Exception("Could not found port mapping info", EZMQX::ServiceUnavailable);
            }
        }

        initialized.store(true);
        terminated.store(false);
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

bool EZMQX::Context::isReverseProxyEnabled()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return reverseProxy;
}

std::string EZMQX::Context::getTnsAddr()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    return tnsAddr;
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

            for (std::list<EZMQX::Publisher*>::iterator itr = publishers.begin(); itr != publishers.end(); itr++)
            {
                if (*itr)
                {
                    (*itr)->terminateOwnResource();
                }
            }

            for (std::list<EZMQX::Subscriber*>::iterator itr = subscribers.begin(); itr != subscribers.end(); itr++)
            {
                if (*itr)
                {
                    (*itr)->terminateOwnResource();
                }
            }

            if (keepAlive)
            {
                EZMQX_LOG_V(DEBUG, TAG, "%s try delete KeepAlive", __func__);
                delete keepAlive;
                keepAlive = nullptr;
            }

            // release resource
            EZMQX_LOG_V(DEBUG, TAG, "%s try release resources", __func__);
            ports.clear();
            usedPorts.clear();
            amlRepDic.clear();
            hostname.clear();
            hostAddr.clear();
            anchorAddr.clear();
            tnsAddr.clear();
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

        if (!keepAlive && isTnsEnabled() && (getKeepAliveInterval() > 0))
        {
            keepAlive = new EZMQX::KeepAlive(this->tnsAddr, getKeepAliveInterval());
        }
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

int EZMQX::Context::updateKeepAliveInterval(int keepAliveInterval)
{
    interval.store(keepAliveInterval);
    return interval.load();
}

int EZMQX::Context::getKeepAliveInterval()
{
    return interval.load();
}

void EZMQX::Context::registerPublisher(EZMQX::Publisher* publisher)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (nullptr == publisher)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s invalid pointer", __func__);
            return;
        }

        publishers.push_back(publisher);
    }

    return;
}

void EZMQX::Context::unregisterPublisher(EZMQX::Publisher* publisher)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (nullptr == publisher)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s invalid pointer", __func__);
            return;
        }

        auto itr = std::find(publishers.begin(), publishers.end(), publisher);
        if (itr != publishers.end())
        {
            publishers.erase(itr);
        }
    }

    return;
}

void EZMQX::Context::registerSubscriber(EZMQX::Subscriber* subscriber)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (nullptr == subscriber)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s invalid pointer", __func__);
            return ;
        }

        subscribers.push_back(subscriber);
    }

    return;
}

void EZMQX::Context::unregisterSubscriber(EZMQX::Subscriber* subscriber)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    {
        std::lock_guard<std::mutex> scopedLock(lock);

        if (nullptr == subscriber)
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s invalid pointer", __func__);
            return;
        }

        auto itr = std::find(subscribers.begin(), subscribers.end(), subscriber);
        if (itr != subscribers.end())
        {
            subscribers.erase(itr);
        }
    }

    return;
}
void EZMQX::Context::getImageName(const std::string& tnsConfPathRef)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered with %s", __func__, tnsConfPathRef.c_str());

    try
    {
        std::ifstream _file(tnsConfPathRef);
        std::string tnsConfInfo((std::istreambuf_iterator<char>(_file)),std::istreambuf_iterator<char>());
        tnsConfInfo = tnsConfInfo.substr(0, tnsConfInfo.size()-1);
        
        std::string errors;
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(tnsConfInfo.c_str(), tnsConfInfo.c_str() + tnsConfInfo.size(), &root, &errors))
        {
            EZMQX_LOG_V(ERROR, TAG, "%s Could not parse json", __func__);
        }
        else
        {
            imageName = root[IMG_NAME].asString();
        }
    }
    catch(...)
    {
        EZMQX_LOG_V(ERROR, TAG, "%s Could not found tns image name", __func__);
        throw EZMQX::Exception("Could not found tns image name", EZMQX::ServiceUnavailable);
    }
}
