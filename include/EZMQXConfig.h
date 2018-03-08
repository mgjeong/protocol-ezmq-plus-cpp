#ifndef EZMQ_X_CONFIG_H
#define EZMQ_X_CONFIG_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <list>

namespace EZMQX {

class Config
{
    private:
        std::mutex lock;
        std::atomic_bool initialized;
        std::atomic_bool terminated;
        static std::shared_ptr<EZMQX::Config> _instance;

        void initialize();
        void terminate();

        // make noncopyable        
        Config();
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;

    public:
        ~Config();
        static std::shared_ptr<EZMQX::Config> getInstance();
        void addAmlModelInfo(const std::list<std::string>& amlModelInfo);
        bool isInitialized();
        bool isTerminated();

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H