#ifndef EZMQ_X_EXCEPTION_H
#define EZMQ_X_EXCEPTION_H

#include <EZMQXErrorCode.h>
#include <stdexcept>
#include <string>

namespace EZMQX
{
/**
* @class Exception
* Exception class that contains ErrorCode and Message.
*
*/
class Exception : public std::runtime_error
{

private:
    ErrorCode code;
public:
    /**
    * Constructor
    *
    * @param msg Message ralated exception
    * @param errCode Specific error code about exception
    * @see ErrorCode
    *
    */
    Exception(const std::string& msg, EZMQX::ErrorCode errCode = UnKnownState)
     : std::runtime_error(msg),  code(errCode)
    {

    }

    /**
    * Return Error Code.
    *
    * @return ErrorCode return specific error code.
    * @see ErrorCode
    *
    */
    ErrorCode getErrCode() const
    {
        return code;
    }
};

} // namespace EZMQX

#endif //EZMQ_X_EXCEPTION_H