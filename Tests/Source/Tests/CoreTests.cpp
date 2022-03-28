#include "Core.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace RightEngine;

class CoreTests : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        testWindow = Window::Create("Test window", 50, 50);
    }

    static void TearDownTestSuite()
    {
        delete testWindow;
    }

    static Window *testWindow;
};

Window* CoreTests::testWindow = nullptr;

TEST_F(CoreTests, TestLogging)
{
    EXPECT_NO_FATAL_FAILURE(R_CORE_INFO("Testing core logging!"));
    EXPECT_NO_FATAL_FAILURE(R_INFO("Testing logging!"));
}

TEST_F(CoreTests, TestWindowCreating)
{
    Window *window = nullptr;
    EXPECT_NO_FATAL_FAILURE(window = Window::Create("Window create test", 10, 10));
    delete window;
}

TEST_F(CoreTests, TestWindowGetSize)
{
    const uint32_t width = 50;
    const uint32_t height = 50;

    EXPECT_EQ(width, testWindow->GetWidth());
    EXPECT_EQ(height, testWindow->GetHeight());
}

