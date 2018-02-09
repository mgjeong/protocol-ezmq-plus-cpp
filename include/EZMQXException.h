#ifndef EZMQ_X_EXCEPTION_H
#define EZMQ_X_EXCEPTION_H

#include <EZMQXErrorCode.h>
#include <stdexcept>
#include <string>

namespace EZMQX
{

class Exception : public std::runtime_error
{

private:
    ErrorCode code;
public:
    Exception(const std::string& msg, EZMQX::ErrorCode errCode = UnKnownState)
     : std::runtime_error(msg),  code(errCode)
    {

    }

    ErrorCode getErrCode() const
    {
        return code;
    }
};

} // namespace EZMQX

#endif //EZMQ_X_EXCEPTION_H