#ifndef EZMQ_X_ERRTYPE_H
#define EZMQ_X_ERRTYPE_H

namespace EZMQX
{
    typedef enum
    {
        OK = 0,
        InvalidParam,
        Initialized,
        NotInitialized,
        Terminated,
        UnKnownState,
        ServiceUnavailable,
        InvalidTopic,
        DuplicatedTopic,
        UnknownTopic,
        InvalidEndpoint,
        BrokenPayload,
        RestError,
        MaximumPortExceed,
        ReleaseWrongPort,
        NoTopicMatched,
        TnsNotAvailable
    }ErrorCode;
}

#endif //EZMQ_X_ERRTYPE_H