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


#ifndef EZMQ_X_FAKE_H
#define EZMQ_X_FAKE_H

#include <map>
#include <string>
#include <EZMQXEndpoint.h>
#include <EZMQXContext.h>

static const std::string FAKE_HOST_NAME = "localhost";
static const std::string FAKE_HOST_ADDR = "localhost";
static const std::string FAKE_REMOTE_ADDR = "localhost";
static const int LOCAL_PORT_START = 4000;
static const int LOCAL_PORT_MAX = 100;

namespace EZMQX {
class FakeSingletonAccessor
{
    public:
        static void setFake()
        {
            EZMQX::Context* ctx = EZMQX::Context::getInstance();
            ctx->setHostInfo(FAKE_HOST_NAME, FAKE_HOST_ADDR);
            ctx->setTnsInfo(FAKE_REMOTE_ADDR,"../../tnsConf.json");

            for (int i = LOCAL_PORT_START; i <= LOCAL_PORT_START + LOCAL_PORT_MAX; i++)
            {
                // set dummy
                ctx->ports[i] = i;
            }

            ctx->initialized.store(true);

            return;
        }
};

} // namespace EZMQX

#endif //EZMQ_X_FAKE_H