#ifndef EZMQ_X_KEEP_ALIVE_H
#define EZMQ_X_KEEP_ALIVE_H

#include <string>
#include <thread>
#include <list>

namespace EZMQX {

class BlockingQue;
typedef enum
{
    TopicKeepAlive,
    UnregisterTopic
}TaskOption;

class KeepAlive
{
private:
    EZMQX::BlockingQue* que;
    std::thread queThread;
    std::thread timerThread;
    std::string remoteAddr;
    KeepAlive();
    void queHandler();
    void timerHandler();

public:
    void inQue(EZMQX::TaskOption opt, std::string payload);
    KeepAlive(std::string addr);
    ~KeepAlive();
};

} // namespace EZMQX

#endif //EZMQ_X_KEEP_ALIVE_H