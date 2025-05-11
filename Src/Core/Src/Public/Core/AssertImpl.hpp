#pragma once

#include <string_view>

namespace core
{
void Assert(std::string_view tag, std::string_view code, std::string_view message = "");
} // core