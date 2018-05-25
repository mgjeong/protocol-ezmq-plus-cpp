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