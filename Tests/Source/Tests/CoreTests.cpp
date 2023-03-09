#include "Core.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace RightEngine;

TEST(CoreTests, PathAlias)
{
	const std::string engineResPath = "/Engine/Shaders/test.vert";
	const std::string gameResPath = "/Shaders/test.vert";

	EXPECT_EQ(Path::Absolute(engineResPath) == G_ENGINE_ASSET_DIR +)
}
