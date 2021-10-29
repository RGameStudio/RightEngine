#include <gtest/gtest.h>
#include <Core.h>

TEST(CoreTests, TestLogging)
{
    EXPECT_NO_FATAL_FAILURE(R_CORE_INFO("Testing core logging!"));
    EXPECT_NO_FATAL_FAILURE(R_INFO("Testing logging!"));
}

