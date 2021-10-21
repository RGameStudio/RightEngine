#pragma once

#include <gtest/gtest.h>

class CoreTests: public ::testing::Test
{};

TEST_F(CoreTests, TestLogging)
{
    EXPECT_NO_FATAL_FAILURE(R_CORE_INFO("Running loggin test!"));
}
