#ifndef EXMQ_X_PUBLISHER_H
#define EXMQ_X_PUBLISHER_H

//#include <AML.h>
#include <string>
#include <memory>
#include <atomic>
#include <EZMQXContext.h>
#include <EZMQPublisher.h>

namespace EZMQX {

class Publisher
{
    private:

        std::shared_ptr<EZMQX::Context> ctx;
        std::shared_ptr<ezmq::EZMQPublisher> pubCtx;

        // delete default ctor
        Publisher();
        Publisher(std::string &topic);

        // make noncopyable        
        Publisher(const Publisher&) = delete;
        Publisher& operator = (const Publisher&) = delete;

        void initialize();

    public:
        static std::shared_ptr<EZMQX::Publisher> getPublisher(std::string &topic);
        void publish(void* object);
        bool isTerminated();
        void terminate();
};
} // namespace EZMQX

#endif //EXMQ_X_PUBLISHER_H