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
    std::atomic_bool terminate;

public:
    BlockingQue(): que(), mlock(), cond(), terminate(false){}

    ~BlockingQue()
    {
        while(!que.empty())
        {
            que.pop();
        }
    }

    void inQue(const std::pair<std::string, std::string> &payload)
    {
        std::lock_guard<std::mutex> lock(mlock);

        if (terminate.load())
        {
            cond.notify_all();
            return;
        }

        que.push(payload);
        cond.notify_all();
        return;
    }

    void deQue(std::pair<std::string, std::string> &payload)
    {
        std::unique_lock<std::mutex> lock(mlock);
        while(terminate.load() == false && que.empty())
        {
            cond.wait(lock);
        }

        if (terminate.load())
        {
            return;
        }

        payload = que.front();
        que.pop();
        return;
    }

    void stop()
    {
        terminate.store(true);
        cond.notify_all();
    }

    bool isTerminated()
    {
        return terminate.load();
    }
};

} // namespace EZMQX

#endif // EZMQ_X_BLOCKING_QUE_H