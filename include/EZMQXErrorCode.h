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


/**
* @file
*
* This file contains error codes of EZMQX stack.
*/

#ifndef EZMQ_X_ERRTYPE_H
#define EZMQ_X_ERRTYPE_H

namespace EZMQX
{
    /**
    * @enum ErrorCode
    * Information of types for ErrorCode, that included in EZMQX::Exception.
    * @see EZMQX::Exception
    */
    typedef enum
    {
        OK = 0,                 /**< enum  Success status code. */
        InvalidParam,           /**< enum  Invalid parameter. */
        Initialized,            /**< enum  Stack is initialized. */
        NotInitialized,         /**< enum  Stack is not initialized. */
        Terminated,             /**< enum  Stack is terminated. */
        UnKnownState,           /**< enum  UnkownState. */
        ServiceUnavailable,     /**< enum  Could not initialize with given info. */
        InvalidTopic,           /**< enum  Invalid topic that validation failed. */
        DuplicatedTopic,        /**< enum  Topic is duplicated. */
        UnknownTopic,           /**< enum  Unknown Topic. */
        InvalidEndpoint,        /**< enum  Invalid Endpoint. */
        BrokenPayload,          /**< enum  Payload is broken. */
        RestError,              /**< enum  Rest(curl) error. */
        MaximumPortExceed,      /**< enum  Maximum port exceed. */
        ReleaseWrongPort,       /**< enum  Try release wrong port. */
        NoTopicMatched,         /**< enum  Could not find topic from tns service. */
        TnsNotAvailable,        /**< enum  Tns not available. */
        UnknownAmlModel,        /**< enum  Could not find AML Model. */
        InvalidAmlModel,        /**< enum  Could not parse AML Model. */
        SessionUnavailable      /**< enum  Could not connect to given endpoint. */
    }ErrorCode;
}

#endif //EZMQ_X_ERRTYPE_H