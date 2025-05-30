#pragma once

#include <Core/Config.hpp>
#include <string_view>

namespace core
{
void CORE_API Assert(std::string_view tag, std::string_view code, std::string_view message = "");
} // core