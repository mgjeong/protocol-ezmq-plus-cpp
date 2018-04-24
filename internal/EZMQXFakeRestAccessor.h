#ifndef EZMQ_X_FAKE_REST_H
#define EZMQ_X_FAKE_REST_H

#include <string>
#include <EZMQXRest.h>
#include <EZMQXLogger.h>

#define TAG "EZMQXFakeRest"

static const std::string GET = "GET";
static const std::string PUT = "PUT";
static const std::string POST = "POST";
static const std::string DELETE = "DELETE";
static const std::string EMPTY = "";

static const std::string QUERY_PARAM = "topic=";

static const std::string UNITTEST_DISCOVERY_FAKE = "/FakeDiscoveryTest";

namespace EZMQX {

class FakeRest : public rest
{
private:
    RestResponse fake(const std::string& op, const std::string& url, const std::string& query, const std::string& payload)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

        RestResponse resp;

        // fake here
        if (op.compare(GET) == 0)
        {
            if (query.compare(QUERY_PARAM+UNITTEST_DISCOVERY_FAKE) == 0)
            {
                RestResponse fake(EZMQX::Success, "[{\"id\":\"5adede045d62b2001ee73dfc\",\"topic\":\"/A/A\",\"endpoint\":\"localhost:4000\",\"schema\":\"GTC_Robot_0.0.1\"},{\"id\":\"5adede045d62b2001ee73dfd\",\"topic\":\"/A/B\",\"endpoint\":\"localhost:4001\",\"schema\":\"GTC_Robot_0.0.1\"},{\"id\":\"5adede045d62b2001ee73dfe\",\"topic\":\"/A/C\",\"endpoint\":\"localhost:4002\",\"schema\":\"GTC_Robot_0.0.1\"},{\"id\":\"5adede095d62b2001ee73dff\",\"topic\":\"/B/A\",\"endpoint\":\"localhost:4000\",\"schema\":\"GTC_Robot_0.0.1\"},{\"id\":\"5adede095d62b2001ee73e00\",\"topic\":\"/B/B\",\"endpoint\":\"localhost:4001\",\"schema\":\"GTC_Robot_0.0.1\"},{\"id\":\"5adede095d62b2001ee73e01\",\"topic\":\"/B/C\",\"endpoint\":\"localhost:4002\",\"schema\":\"GTC_Robot_0.0.1\"}]");
                return fake;
            }
        }
        else if (op.compare(PUT) == 0)
        {

        }
        else if (op.compare(POST) == 0)
        {

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
    RestResponse Get(const std::string &url)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(GET, EMPTY, EMPTY, EMPTY);
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

    RestResponse Delete(const std::string &url, const std::string &payload)
    {
        EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
        return fake(DELETE, url, EMPTY, payload);
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