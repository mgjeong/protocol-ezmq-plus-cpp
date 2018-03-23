#ifndef EZMQ_X_REST_H
#define EZMQ_X_REST_H

#include <string>
#include <curl/curl.h>

namespace EZMQX {

class SimpleRest
{
private:
    CURL *curl;
    // make noncopyable
    SimpleRest(const SimpleRest&) = delete;
    SimpleRest& operator = (const SimpleRest&) = delete;
public:
    SimpleRest();
    ~SimpleRest();
    std::string Get(std::string url);
    std::string Get(std::string url, std::string query);
    std::string Put(std::string url);
    std::string Post(std::string url, std::string payload);
    std::string Delete(std::string url);
};

} // namespace EZMQX

#endif //EZMQ_X_REST_H