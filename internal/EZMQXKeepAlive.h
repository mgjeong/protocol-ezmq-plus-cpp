#ifndef EZMQ_X_KEEP_ALIVE_H
#define EZMQ_X_KEEP_ALIVE_H

#include <string>
#include <thread>

namespace EZMQX {

class BlockingQue;

typedef enum
{
    KeepAlive,
    UnregisterTopic
}TaskOption;

class KeepAlive
{
private:
    EZMQX::BlockingQue* que;
    std::thread mThread;
    void handler();

public:
    void inQue(TaskOption opt, std::string payload);
    KeepAlive();
    ~KeepAlive();
};

} // namespace EZMQX

#endif //EZMQ_X_KEEP_ALIVE_H