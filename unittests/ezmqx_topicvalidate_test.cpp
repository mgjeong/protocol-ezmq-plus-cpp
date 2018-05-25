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

#define NAGATIVE_TC_SIZE 16
#define POSITIVE_TC_SIZE 6

static std::string NAGATIVE_TC[NAGATIVE_TC_SIZE] = {
    "/",
    "//////)///(///////////",
    "//3/1/2/1/2/3",
    "312123/",
    "/fds+dsfg-23-/",
    "/312?_!_12--3/",
    "/312_!_123/sda",
    "/        /312123",
    "-=0/ssdfdfadsf/fdsg-/-0-",
    "/sdfsdf*/",
    "/sdf/sdf/sdfgfg/",
    "sdf/sdf/sdf/sdfgfg/",
    "/sdf/sdf/sdf gfg/12",
    "/sdfsdf/123sdfs/t312xsdf*/213lkj_+/",
    "sdfsdf/123sdfs/t312xsdf*/213lkj_+/",
    "/sdfsdf/123sdfs//t312xsdf*/213lkj_+"
};


static std::string POSITIVE_TC[POSITIVE_TC_SIZE] = {
    "/sdf/sdf/sdfgfg/12",
    "/sdfsdf/123sdfs/t312xsdf/213lkj_",
    "/-0/ssd............fdfadsf/fdsg-/-0-",
    "/sdf/sdf/sdfgfg",
    "/*/-/*/-/-/-______/-/-/-3223434223424323421",
    "/123/sdafdsaf/44___kk/2232/abicls"
};

namespace
{

TEST_F(TopicTest, Negative1)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[0]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative2)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[1]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative3)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[2]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative4)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[3]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative5)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[4]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative6)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[5]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative7)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[6]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative8)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[7]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative9)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[8]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative10)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[9]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative11)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[10]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative12)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[11]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative13)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[12]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative14)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[13]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative15)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[14]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, Negative16)
{
    EXPECT_TRUE(topicTest(NAGATIVE_TC[15]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, POSITIVE1)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[0]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, POSITIVE2)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[1]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, POSITIVE3)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[2]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, POSITIVE4)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[3]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, POSITIVE5)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[4]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, POSITIVE6)
{
    EXPECT_TRUE(topicTest(POSITIVE_TC[5]) == EZMQX::TnsNotAvailable);
}

}