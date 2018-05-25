/*******************************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *******************************************************************************/


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
    int interval;
    KeepAlive();
    void queHandler();
    void timerHandler();
    void stopTimer();

public:
    void inQue(EZMQX::TaskOption opt, std::string payload);
    KeepAlive(std::string addr, int KeepAliveInterval);
    ~KeepAlive();
};

} // namespace EZMQX

#endif //EZMQ_X_KEEP_ALIVE_H