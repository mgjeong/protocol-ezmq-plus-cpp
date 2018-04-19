#ifndef EZMQ_X_CONFIG_H
#define EZMQ_X_CONFIG_H

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <list>

namespace EZMQX {

class Context;

typedef enum
{
    StandAlone,
    Docker
}ModeOption;

class Config
{
    private:
        std::mutex lock;
        std::atomic_bool initialized;
        ModeOption configMode;
        EZMQX::Context* ctx;
        virtual void initialize();
        virtual void terminate();

        // make noncopyable        
        Config();
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;

    public:
        Config(ModeOption mode);
        ~Config();
        void setHostInfo(std::string hostName, std::string hostAddr);
        void setTnsInfo(std::string remoteAddr);
        std::list<std::string> addAmlModel(const std::list<std::string>& amlFilePath);
        void reset(ModeOption mode);

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H