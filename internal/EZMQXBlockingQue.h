#ifndef EZMQ_X_BLOCKING_QUE_H
#define EZMQ_X_BLOCKING_QUE_H

#include <string>
#include <list>
#include <queue>
#include <memory>
#include <atomic>
#include <condition_variable>

namespace EZMQX {

class BlockingQue
{
private:
    std::queue<std::pair<std::string, std::string>> que;
    std::mutex mlock;
    std::condition_variable cond;

public:
    BlockingQue(): que(), mlock(), cond(){}

    ~BlockingQue(){}

    void inQue(const std::pair<std::string, std::string> &payload)
    {
        std::lock_guard<std::mutex> lock(mlock);
        que.push(payload);
        cond.notify_all();
        return;
    }

    void deQue(std::pair<std::string, std::string> &payload)
    {
        std::unique_lock<std::mutex> lock(mlock);
        while(que.empty())
        {
            cond.wait(lock);
        }
        payload = que.front();
        que.pop();
        return;
    }
};

} // namespace EZMQX

#endif // EZMQ_X_BLOCKING_QUE_H