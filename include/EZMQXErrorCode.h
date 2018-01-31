#ifndef EZMQ_X_ERRTYPE_H
#define EZMQ_X_ERRTYPE_H

namespace EXMQX
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
        InvalidEndpoint
    }ErrorCode;
}

#endif //EZMQ_X_ERRTYPE_H