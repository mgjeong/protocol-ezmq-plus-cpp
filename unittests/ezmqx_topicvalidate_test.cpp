#include <gtest.h>
#include <gmock.h>
#include <iostream>
#include <EZMQXConfig.h>
#include <EZMQXException.h>
#include <UnitTestHelper.h>
#include <string>

#define FALSE_TC_SIZE 16
#define TRUE_TC_SIZE 6

static std::string FALSE_TC[FALSE_TC_SIZE] = {
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


static std::string TRUE_TC[TRUE_TC_SIZE] = {
    "/sdf/sdf/sdfgfg/12",
    "/sdfsdf/123sdfs/t312xsdf/213lkj_",
    "/-0/ssd............fdfadsf/fdsg-/-0-",
    "/sdf/sdf/sdfgfg",
    "/*/-/*/-/-/-______/-/-/-3223434223424323421",
    "/123/sdafdsaf/44___kk/2232/abicls"
};

namespace
{

TEST_F(TopicTest, FAIL1)
{
    EXPECT_TRUE(topicTest(FALSE_TC[0]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL2)
{
    EXPECT_TRUE(topicTest(FALSE_TC[1]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL3)
{
    EXPECT_TRUE(topicTest(FALSE_TC[2]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL4)
{
    EXPECT_TRUE(topicTest(FALSE_TC[3]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL5)
{
    EXPECT_TRUE(topicTest(FALSE_TC[4]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL6)
{
    EXPECT_TRUE(topicTest(FALSE_TC[5]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL7)
{
    EXPECT_TRUE(topicTest(FALSE_TC[6]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL8)
{
    EXPECT_TRUE(topicTest(FALSE_TC[7]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL9)
{
    EXPECT_TRUE(topicTest(FALSE_TC[8]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL10)
{
    EXPECT_TRUE(topicTest(FALSE_TC[9]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL11)
{
    EXPECT_TRUE(topicTest(FALSE_TC[10]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL12)
{
    EXPECT_TRUE(topicTest(FALSE_TC[11]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL13)
{
    EXPECT_TRUE(topicTest(FALSE_TC[12]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL14)
{
    EXPECT_TRUE(topicTest(FALSE_TC[13]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL15)
{
    EXPECT_TRUE(topicTest(FALSE_TC[14]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, FAIL16)
{
    EXPECT_TRUE(topicTest(FALSE_TC[15]) == EZMQX::InvalidTopic);
}

TEST_F(TopicTest, PASS1)
{
    EXPECT_TRUE(topicTest(TRUE_TC[0]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, PASS2)
{
    EXPECT_TRUE(topicTest(TRUE_TC[1]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, PASS3)
{
    EXPECT_TRUE(topicTest(TRUE_TC[2]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, PASS4)
{
    EXPECT_TRUE(topicTest(TRUE_TC[3]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, PASS5)
{
    EXPECT_TRUE(topicTest(TRUE_TC[4]) == EZMQX::TnsNotAvailable);
}

TEST_F(TopicTest, PASS6)
{
    EXPECT_TRUE(topicTest(TRUE_TC[5]) == EZMQX::TnsNotAvailable);
}

}