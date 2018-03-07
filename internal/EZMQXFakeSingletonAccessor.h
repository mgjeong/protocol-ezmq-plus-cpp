#ifndef EZMQ_X_FAKE_H
#define EZMQ_X_FAKE_H

#include <map>
#include <string>
#include <EZMQXEndpoint.h>

namespace EZMQX {
class FakeSingletonAccessor
{
    public:
        static void setFake();
};

} // namespace EZMQX

#endif //EZMQ_X_FAKE_H