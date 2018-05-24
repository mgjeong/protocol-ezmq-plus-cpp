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
        ServiceUnavailable,     /**< enum  Could not initialize with given tns info. */
        InvalidTopic,           /**< enum  Invalid topic that validation failed. */
        DuplicatedTopic,        /**< enum  Topic is duplicated. */
        UnknownTopic,           /**< enum  Unknown Topic. */
        InvalidEndpoint,        /**< enum  Invalid Endpoint. */
        BrokenPayload,          /**< enum  Payload is broken. */
        RestError,              /**< enum  Rest(curl) error. */
        MaximumPortExceed,      /**< enum  Maximum port exceed. */
        ReleaseWrongPort,       /**< enum  Try release wrong port. */
        NoTopicMatched,         /**< enum  Could not find topic from tns service. */
        TnsNotAvailable         /**< enum  Tns not available. */
    }ErrorCode;
}

#endif //EZMQ_X_ERRTYPE_H