#include <EZMQXRest.h>
#include <string>

static const std::string DELETE = "DELETE";
static const std::string PUT = "PUT";

static size_t _writeCb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    size_t ret = size*nmemb;
    ((std::string*)userdata)->append((char*)ptr, size*nmemb);
    return ret;
}

EZMQX::SimpleRest::SimpleRest() : curl(nullptr)
{
    curl = curl_easy_init();
    if (!curl)
    {
        curl = curl_easy_init();
    }

    if (!curl)
    {
        // throw exception
    }

}

EZMQX::SimpleRest::~SimpleRest()
{
    curl_easy_cleanup(curl);
}

std::string EZMQX::SimpleRest::Get(std::string url)
{
    std::string buff;
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        // std::cout<<"curl res != CURLE_OK"<<std::endl;
        // std::cout<<"curl res is " <<" "<<curl_easy_strerror(res)<<std::endl;
        //throw exception
    }

    return buff;
}

std::string EZMQX::SimpleRest::Get(std::string url, std::string query)
{
    std::string buff;
    std::string addr = url + "?" + query;
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, addr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        // std::cout<<"curl res != CURLE_OK"<<std::endl;
        // std::cout<<"curl res is " <<" "<<curl_easy_strerror(res)<<std::endl;
        //throw exception
    }

    return buff;
}

std::string EZMQX::SimpleRest::Put(std::string url, std::string payload)
{
    CURLcode res;
    std::string buff;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, PUT.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(payload.size()));
    if(res != CURLE_OK)
    {

    }

    return buff;
}

std::string EZMQX::SimpleRest::Post(std::string url, std::string payload)
{
    std::string buff;
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)(payload.size()));
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {
        // std::cout<<"curl res != CURLE_OK"<<std::endl;
        // std::cout<<"curl res is " <<" "<<curl_easy_strerror(res)<<std::endl;
        //throw exception
    }

    return buff;

}

std::string EZMQX::SimpleRest::Delete(std::string url)
{
    CURLcode res;
    std::string buff;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, DELETE.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buff);
    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
    {

    }

    return buff;
}