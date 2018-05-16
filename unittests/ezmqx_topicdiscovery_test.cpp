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
    }

}