#ifndef EZMQ_X_SUBSCRIBER_H
#define EZMQ_X_SUBSCRIBER_H

#include <string>
#include <list>
#include <queue>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <EZMQXErrorCode.h>
#include <EZMQXTopic.h>
#include <EZMQSubscriber.h>
#include <AMLInterface.h>
#include <Representation.h>
#include <EZMQXAmlModelInfo.h>

namespace EZMQX
{

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

typedef std::function<void(std::string topic, const AMLObject& payload)> SubCb;
typedef std::function<void(std::string topic, EZMQX::ErrorCode errCode)> SubErrCb;

class Subscriber
{
    private:
        std::mutex lock;
        std::atomic_bool terminated;
        std::list<std::shared_ptr<ezmq::EZMQSubscriber>> subscribers;
        std::map<std::string, std::shared_ptr<Representation>> repDic;
        std::string token;
        EZMQX::SubCb mSubCb;
        EZMQX::SubErrCb mSubErrCb;
        BlockingQue que;
        std::thread mThread;
        void handler();
        void internalSubCb(std::string topic, const ezmq::EZMQMessage &event);
        void initialize(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        virtual void verifyTopics(const std::list<std::string> &topics, std::list<EZMQX::Topic> &verified);
        virtual void verifyTopics(const std::list<EZMQX::Topic> &topics);

        // delete default ctor
        Subscriber();
        Subscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        Subscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        // make noncopyable
        Subscriber(const Subscriber&) = delete;
        Subscriber& operator = (const Subscriber&) = delete;

    public:
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::string &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::list<std::string> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const EZMQX::Topic &topic, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        static std::shared_ptr<EZMQX::Subscriber> getSubscriber(const std::list<EZMQX::Topic> &topics, EZMQX::SubCb &subCb, EZMQX::SubErrCb &errCb);
        bool isTerminated();
        void terminate();
        std::list<EZMQX::Topic> getTopics();
};

}

#endif //EZMQ_X_SUBSCRIBER_H