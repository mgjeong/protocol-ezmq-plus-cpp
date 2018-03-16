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
        static std::shared_ptr<EZMQX::Config> _instance;

        virtual void initialize();
        virtual void terminate();

        // make noncopyable        
        Config();
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;

    public:
        ~Config();
        static std::shared_ptr<EZMQX::Config> getInstance();
        std::list<std::string> addAmlModel(const std::list<std::string>& amlFilePath);

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H