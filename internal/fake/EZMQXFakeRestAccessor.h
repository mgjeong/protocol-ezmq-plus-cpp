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


#ifndef EZMQ_X_FAKE_REST_H
#define EZMQ_X_FAKE_REST_H

#include <string>
#include <EZMQXRest.h>
#include <EZMQXLogger.h>
#include <EZMQXContext.h>

#define TAG "EZMQXFakeRest"

static const std::string GET = "GET";
static const std::string PUT = "PUT";
static const std::string POST = "POST";
static const std::string DELETE = "DELETE";
static const std::string EMPTY = "";

static const std::string QUERY_PARAM = "name=";

static const std::string UNITTEST_DISCOVERY_FAKE = "/FakeDiscoveryTest&hierarchical=yes";

// publish stub
static const std::string TNS_KNOWN_PORT = "48323";
static const std::string COLLON = ":";
static const std::string PREFIX = "/api/v1";
static const std::string TOPIC = "/tns/topic";

namespace EZMQX {

class FakeRest : public rest
{
private:
    RestResponse fake(const std::string& op, const std::string& url, const std::string& query, const std::string& payload)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s %s Entered url is  %s query is %s", __func__, op.c_str(), url.c_str(), query.c_str());

        if (!payload.empty())
        {
            EZMQX_LOG_V(DEBUG, TAG, "%s %s payload is %s", __func__, op.c_str(), payload.c_str());
        }

        RestResponse resp;

        // fake here
        if (op.compare(GET) == 0)
        {
            if (query.compare(QUERY_PARAM+UNITTEST_DISCOVERY_FAKE) == 0)
            {
                RestResponse fake(EZMQX::Success, "{ \"topics\" : [{ \"name\" : \"/A/A\", \"endpoint\" : \"localhost:4000\", \"datamodel\" : \"GTC_Robot_0.0.1\", \"secured\" : false }, { \"name\" : \"/A/B\", \"endpoint\" : \"localhost:4001\", \"datamodel\" : \"GTC_Robot_0.0.1\", \"secured\" : false} , { \"name\" : \"/A/C\", \"endpoint\" : \"localhost:4002\", \"datamodel\" : \"GTC_Robot_0.0.1\", \"secured\" : false }, { \"name\" : \"/B/A\", \"endpoint\" : \"localhost:4000\", \"datamodel\" : \"GTC_Robot_0.0.1\" ,\"secured\" : false }, { \"name\" : \"/B/B\", \"endpoint\" : \"localhost:4001\", \"datamodel\" : \"GTC_Robot_0.0.1\", \"secured\" : false  }, { \"name\" : \"/B/C\", \"endpoint\" : \"localhost:4002\", \"datamodel\" : \"GTC_Robot_0.0.1\", \"secured\" : false }]}");
                return fake;
            }
        }
        else if (op.compare(PUT) == 0)
        {

        }
        else if (op.compare(POST) == 0)
        {
            // publish register stub
            if (url.compare(EZMQX::Context::getInstance()->getTnsAddr() + PREFIX + TOPIC) == 0)
            {
                RestResponse fake(EZMQX::Created, "{ \"ka_interval\" : 180 }");
                return fake;
            }

        }
        else if (op.compare(DELETE) == 0)
        {

        }
        else
        {
            EZMQX_LOG_V(ERROR, TAG, "%s unexpected arguments", __func__);
        }

        return resp;
    }
public:
    FakeRest(){};
    ~FakeRest(){};
    RestResponse Get(const std::string &url)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(GET, url, EMPTY, EMPTY);
    }

    RestResponse Get(const std::string &url, const std::string &query)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(GET, url, query, EMPTY);
    }

    RestResponse Put(const std::string &url, const std::string &payload)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(PUT, url, EMPTY, payload);
    }

    RestResponse Post(const std::string &url, const std::string &payload)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(POST, url, EMPTY, payload);
    }

    RestResponse Delete(const std::string &url, const std::string &query)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(DELETE, url, query, EMPTY);
    }

};

class FakeRestFactory : public RestFactoryInterface
{
public:
    rest* getSomeRest()
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return new FakeRest;
    }
};

class FakeRestAccessor
{
public:
    static void setFake()
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        RestFactoryInterface* factory = RestService::factory;
        if (factory)
        {
            delete factory;
            factory = nullptr;
        }
        RestService::factory = new FakeRestFactory();
    }
};

} // namespace EZMQX
#endif //EZMQ_X_FAKE_REST_H