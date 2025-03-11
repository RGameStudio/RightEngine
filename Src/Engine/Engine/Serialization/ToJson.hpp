#pragma once

#include <Engine/Config.hpp>
#include <nlohmann/json.hpp>

namespace engine
{
ENGINE_API std::string ToJsonString(const rttr::variant& obj, rttr::type expectedType = {});
ENGINE_API nlohmann::json ToJsonObject(const rttr::variant& obj, rttr::type expectedType = {});
} // engine