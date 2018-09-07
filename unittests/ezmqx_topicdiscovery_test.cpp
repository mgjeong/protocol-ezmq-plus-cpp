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


#include <gtest.h>
#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <string>
#include <list>

TEST_F(FakeDiscoveryTest, FakeRest)
{
    std::list<EZMQX::Topic> topics = discovery->hierarchicalQuery("/FakeDiscoveryTest");


    std::list<EZMQX::Topic> checkList = getDummyTopics();

    auto checker = checkList.begin();
    for (auto itr =  topics.begin();  itr != topics.end(); itr++, checker++)
    {
        EXPECT_TRUE((*itr).getName().compare((*checker).getName())==0);
        EXPECT_TRUE((*itr).getDatamodel().compare((*checker).getDatamodel())==0);
        EXPECT_TRUE((*itr).getEndpoint().toString().compare((*checker).getEndpoint().toString()) == 0);
        EXPECT_TRUE((*itr).isSecured() == false);

    }

}