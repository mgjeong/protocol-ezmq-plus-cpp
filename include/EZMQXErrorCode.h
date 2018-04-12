#ifndef EZMQ_X_ERRTYPE_H
#define EZMQ_X_ERRTYPE_H

namespace EZMQX
{
    typedef enum
    {
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
        ReleaseWrongPort

    }ErrorCode;
}

#endif //EZMQ_X_ERRTYPE_H