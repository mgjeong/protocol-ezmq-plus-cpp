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
        EZMQX::Context* ctx;
        void setTnsInfo(std::string remoteAddr);
        void initialize(EZMQX::ModeOption configMode);
        void terminate();

        // make noncopyable
        Config();
        Config(const Config&) = delete;
        Config& operator = (const Config&) = delete;
        ~Config();

    public:
        static Config* getInstance();
        void startDockerMode();
        void startStandAloneMode(bool useTns, std::string tnsAddr);
        std::list<std::string> addAmlModel(const std::list<std::string>& amlFilePath);
        void reset();

};

} // namespace EZMQX

#endif //EZMQ_X_CONFIG_H