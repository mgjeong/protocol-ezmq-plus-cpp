#ifndef EZMQ_X_KEEP_ALIVE_H
#define EZMQ_X_KEEP_ALIVE_H

#include <string>
#include <thread>
#include <list>
#include <condition_variable>
#include <atomic>

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
    std::mutex timerMutex;
    std::condition_variable timerCond;
    std::atomic_bool timerIsTerminate;
    std::string remoteAddr;
    KeepAlive();
    void queHandler();
    void timerHandler();
    void stopTimer();

public:
    void inQue(EZMQX::TaskOption opt, std::string payload);
    KeepAlive(std::string addr);
    ~KeepAlive();
};

} // namespace EZMQX

#endif //EZMQ_X_KEEP_ALIVE_H