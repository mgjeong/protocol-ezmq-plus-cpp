#ifndef EZMQ_X_REST_H
#define EZMQ_X_REST_H

#include <string>
#include <curl/curl.h>

namespace EZMQX {

typedef enum
{   WrongCode = 0,
    Success = 200,
    Created = 201,
    Accepted = 202,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    NotAllowed = 405,
    Conflict = 409,
    InternalError = 500,
    Unabailable = 503
}HttpStatus;

class RestResponse
{
private:
    HttpStatus status;
    std::string payload;
    RestResponse(){};
public:
    RestResponse(HttpStatus restStatus, const std::string &restPayload): status(restStatus), payload(restPayload){};
    HttpStatus getStatus(){return status;};
    std::string getPayload(){return payload;};
};

class rest
{
public:
    virtual RestResponse Get(const std::string &url);
    virtual RestResponse Get(const std::string &url, const std::string &query);
    virtual RestResponse Put(const std::string &url, const std::string &payload);
    virtual RestResponse Post(const std::string &url, const std::string &payload);
    virtual RestResponse Delete(const std::string &url, const std::string &payload);
};

class SimpleRest : public rest
{
private:
    CURL *curl;
    // make noncopyable
    SimpleRest(const SimpleRest&) = delete;
    SimpleRest& operator = (const SimpleRest&) = delete;
public:
    SimpleRest();
    ~SimpleRest();
    RestResponse Get(const std::string &url);
    RestResponse Get(const std::string &url, const std::string &query);
    RestResponse Put(const std::string &url, const std::string &payload);
    RestResponse Post(const std::string &url, const std::string &payload);
    RestResponse Delete(const std::string &url, const std::string &payload);
};

// rest Factory
class RestFactory
{
public:
    virtual rest getSomeRest();
};

// fake accessor fot RestService
class FakeRestAccessor;

//singleton
class RestService
{
friend FakeRestAccessor;
private:
    static RestFactory factory;

public:
    static RestResponse Get(const std::string &url);
    static RestResponse Get(const std::string &url, const std::string &query);
    static RestResponse Put(const std::string &url, const std::string &payload);
    static RestResponse Post(const std::string &url, const std::string &payload);
    static RestResponse Delete(const std::string &url, const std::string &payload);
};

} // namespace EZMQX

#endif //EZMQ_X_REST_H