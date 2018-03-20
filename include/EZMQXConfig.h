#ifndef EZMQ_X_CONFIG_H
#define EZMQ_X_CONFIG_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <list>

namespace EZMQX {

typedef enum
{
    StandAlone,
    FullFeature
}ModeOption;

class Config
{
    private:
        std::mutex lock;
        std::atomic_bool initialized;
        ModeOption configMode;
        static std::shared_ptr<EZMQX::Config> _instance;
        virtual void initialize();
        virtual void terminate();

        // make noncopyable        
        Config();
        Config(ModeOption mode);
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;

    public:
        ~Config();
        static std::shared_ptr<EZMQX::Config> getInstance(ModeOption mode);
        void setHostInfo(std::string hostName, std::string hostAddr);
        void setTnsInfo(std::string remoteAddr);
        std::list<std::string> addAmlModel(const std::list<std::string>& amlFilePath);

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H