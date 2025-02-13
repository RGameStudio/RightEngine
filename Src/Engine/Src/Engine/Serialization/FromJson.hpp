#pragma once

#include <Engine/Config.hpp>
#include <simdjson.h>
#include <optional>

namespace engine
{

ENGINE_API rttr::variant FromJsonObject(const simdjson::dom::element& j, rttr::type expectedType = {});
ENGINE_API rttr::variant FromJsonString(const std::string& str, rttr::type expectedType = {}, std::string* errorBuffer = nullptr);

template<typename Result> std::optional<Result> FromJsonString(const std::string& str, std::string* errorBuffer = nullptr)
{
	rttr::variant var = FromJsonString(str, rttr::type::get<Result>(), errorBuffer);
	if constexpr (std::is_same_v<Result, rttr::variant>)
		return var;
	else if (var.is_type<Result>())
		return std::move(var.get_value_unsafe<Result>());
	else
		return {};
}

} // engine