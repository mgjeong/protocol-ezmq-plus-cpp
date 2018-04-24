#include <EZMQXRest.h>
#include <string>
#include <EZMQXException.h>
#include <EZMQXLogger.h>
#define TAG "EZMQXRest"

static const std::string QUESTION_MARK = "?";
static const std::string DELETE = "DELETE";
static const std::string PUT = "PUT";

static size_t _writeCb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    size_t ret = size*nmemb;
    ((std::string*)userdata)->append((char*)ptr, size*nmemb);
    return ret;
}

EZMQX::SimpleRest::SimpleRest() : curl(nullptr)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    curl = curl_easy_init();
    if (!curl)
    {
        curl = curl_easy_init();
    }

    if (!curl)
    {
        throw EZMQX::Exception("Could not initialize curl lib", EZMQX::RestError);
    }

}

EZMQX::SimpleRest::~SimpleRest()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    curl_easy_cleanup(curl);
}

EZMQX::RestResponse EZMQX::SimpleRest::Get(const std::string &url)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);
    std::string buff;
    CURLcode res;
    long respCode;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        throw EZMQX::Exception(curl_easy_strerror(res), EZMQX::RestError);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);

    return RestResponse(static_cast<EZMQX::HttpStatus>(respCode), buff);
}

EZMQX::RestResponse EZMQX::SimpleRest::Get(const std::string &url, const std::string &query)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    std::string buff;
    std::string addr = url + QUESTION_MARK + query;
    CURLcode res;
    long respCode;
    curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        throw EZMQX::Exception(curl_easy_strerror(res), EZMQX::RestError);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);

    return RestResponse(static_cast<EZMQX::HttpStatus>(respCode), buff);
}

EZMQX::RestResponse EZMQX::SimpleRest::Put(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    CURLcode res;
    long respCode;
    std::string buff;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, PUT.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(payload.size()));
    if(res != CURLE_OK)
    {
        throw EZMQX::Exception(curl_easy_strerror(res), EZMQX::RestError);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);

    return RestResponse(static_cast<EZMQX::HttpStatus>(respCode), buff);
}

EZMQX::RestResponse EZMQX::SimpleRest::Post(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    std::string buff;
    CURLcode res;
    long respCode;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(payload.size()));
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        throw EZMQX::Exception(curl_easy_strerror(res), EZMQX::RestError);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);

    return RestResponse(static_cast<EZMQX::HttpStatus>(respCode), buff);

}

EZMQX::RestResponse EZMQX::SimpleRest::Delete(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    CURLcode res;
    std::string buff;
    long respCode;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, DELETE.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(payload.size()));
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        throw EZMQX::Exception(curl_easy_strerror(res), EZMQX::RestError);
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respCode);

    return RestResponse(static_cast<EZMQX::HttpStatus>(respCode), buff);
}

EZMQX::rest* EZMQX::RestFactory::getSomeRest()
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    return new SimpleRest;
}

EZMQX::RestFactoryInterface* EZMQX::RestService::factory = new EZMQX::RestFactory();

EZMQX::RestResponse EZMQX::RestService::Get(const std::string &url)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::rest* ctx = nullptr;
    RestResponse resp;

    try
    {
        ctx = factory->getSomeRest();
        resp = ctx->Get(url);
    }
    catch(const EZMQX::Exception& e)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest request", __func__);
        throw EZMQX::Exception("Could not send rest request", EZMQX::RestError);
    }

    delete ctx;
    return resp;
}

EZMQX::RestResponse EZMQX::RestService::Get(const std::string &url, const std::string &query)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::rest* ctx = nullptr;
    RestResponse resp;

    try
    {
        ctx = factory->getSomeRest();
        resp = ctx->Get(url, query);
    }
    catch(const EZMQX::Exception& e)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest request", __func__);
        throw EZMQX::Exception("Could not send rest request", EZMQX::RestError);
    }

    delete ctx;
    return resp;
}

EZMQX::RestResponse EZMQX::RestService::Put(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::rest* ctx = nullptr;
    RestResponse resp;

    try
    {
        ctx = factory->getSomeRest();
        resp = ctx->Put(url, payload);
    }
    catch(const EZMQX::Exception& e)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest request", __func__);
        throw EZMQX::Exception("Could not send rest request", EZMQX::RestError);
    }

    delete ctx;
    return resp;
}

EZMQX::RestResponse EZMQX::RestService::Post(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::rest* ctx = nullptr;
    RestResponse resp;

    try
    {
        ctx = factory->getSomeRest();
        resp = ctx->Post(url, payload);
    }
    catch(const EZMQX::Exception& e)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest request", __func__);
        throw EZMQX::Exception("Could not send rest request", EZMQX::RestError);
    }

    delete ctx;
    return resp;
}

EZMQX::RestResponse EZMQX::RestService::Delete(const std::string &url, const std::string &payload)
{
    EZMQX_LOG_V(DEBUG, TAG, "%s Entered", __func__);

    EZMQX::rest* ctx = nullptr;
    RestResponse resp;

    try
    {
        ctx = factory->getSomeRest();
        resp = ctx->Delete(url, payload);
    }
    catch(const EZMQX::Exception& e)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s %s", __func__, e.what());
        throw e;
    }
    catch(...)
    {
        if (ctx)
        {
            delete ctx;
        }

        EZMQX_LOG_V(ERROR, TAG, "%s Could not send rest request", __func__);
        throw EZMQX::Exception("Could not send rest request", EZMQX::RestError);
    }

    delete ctx;
    return resp;
}

EZMQX::RestService::~RestService()
{
    if (factory)
    {
        delete factory;
        factory = nullptr;
    }
}