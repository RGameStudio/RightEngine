#include <Engine/Serialization/ToJson.hpp>

namespace
{

bool IsStringLike(rttr::type t)
{
	if (const auto& meta = t.get_metadata(engine::registration::meta::C_METAINFO_IS_STRING_LIKE_TAG); meta.is_valid())
	{
		return true;
	}

	return t.is_enumeration()
		|| t == rttr::type::get<std::string>()
		|| t == rttr::type::get<std::string_view>()
		|| t == rttr::type::get<eastl::vector<uint8_t>>();
}

nlohmann::json WriteArithmeticType(const rttr::instance& obj)
{
	auto t = obj.get_type().get_raw_type();
	ENGINE_ASSERT(t.is_arithmetic());

	if (t == rttr::type::get<bool>())			        return *obj.try_convert<bool>();
	else if (t == rttr::type::get<char>())		        return *obj.try_convert<char>();
	else if (t == rttr::type::get<int8_t>())	        return *obj.try_convert<int8_t>();
	else if (t == rttr::type::get<int16_t>())	        return *obj.try_convert<int16_t>();
	else if (t == rttr::type::get<int32_t>())	        return *obj.try_convert<int32_t>();
	else if (t == rttr::type::get<int64_t>())	        return *obj.try_convert<int64_t>();
	else if (t == rttr::type::get<uint8_t>())	        return *obj.try_convert<uint8_t>();
	else if (t == rttr::type::get<uint16_t>())	        return *obj.try_convert<uint16_t>();
	else if (t == rttr::type::get<uint32_t>())	        return *obj.try_convert<uint32_t>();
	else if (t == rttr::type::get<uint64_t>())	        return *obj.try_convert<uint64_t>();
	else if (t == rttr::type::get<float>())		        return *obj.try_convert<float>();
	else if (t == rttr::type::get<double>())	        return *obj.try_convert<double>();

	ENGINE_ASSERT_WITH_MESSAGE(false, "Unexpected arithmetic type, add support here!");
	return {};
}

void WrapIntoObjectIfNeeded(nlohmann::json& j)
{
	if (!j.is_object())
	{
		nlohmann::json wrapper;
		wrapper["__value__"] = std::move(j);
		j = std::move(wrapper);
	}
}

nlohmann::json ToJsonRecursively(const rttr::variant& obj, rttr::type expectedType)
{
	static const auto UnwrapVariant = [](const rttr::variant& obj, auto&& recurse) -> const rttr::variant&
	{
		if (obj.get_type().get_wrapped_type() == rttr::type::get<rttr::variant>())
		{
			return recurse(obj.get_wrapped_value_unsafe<rttr::variant>(), recurse);
		}

		if (obj.get_type().get_wrapped_type() == rttr::type::get<const rttr::variant>())
		{
			return recurse(obj.get_wrapped_value_unsafe<const rttr::variant>(), recurse);
		}

		return obj;
	};

	const rttr::variant& objUnwrapped = UnwrapVariant(obj, UnwrapVariant);

	auto objType = rttr::try_get_raw_type(rttr::get_type_or_wrapped_type(objUnwrapped));

	nlohmann::json j;

	if (!objType)
	{
		return j;
	}

	if (objType.is_arithmetic())
	{
		auto unwrappedInstance = objUnwrapped.get_type().is_wrapper() ? rttr::instance(objUnwrapped).get_wrapped_instance() : rttr::instance(objUnwrapped);
		j = WriteArithmeticType(unwrappedInstance);
	}
	else if (IsStringLike(objType))
	{
		j = objUnwrapped.to_string();
	}
	else if (objType.is_sequential_container())
	{
		auto view = objUnwrapped.create_sequential_view();
		j = nlohmann::json::array();

		for (const auto& item : view)
		{
			j.push_back(ToJsonRecursively(item, view.get_value_type()));
		}
	}
	else if (objType.is_associative_container())
	{
		auto view = objUnwrapped.create_associative_view();
		if (view.is_key_only_type())
		{
			j = nlohmann::json::array();

			for (auto& [key, value] : view)
			{
				j.push_back(ToJsonRecursively(key, view.get_key_type()));
			}
		}
		else if (IsStringLike(view.get_key_type()))
		{
			j = nlohmann::json::object();

			for (auto& [key, value] : view)
			{
				j[key.to_string()] = ToJsonRecursively(value, view.get_value_type());
			}
		}
		else
		{
			j = nlohmann::json::array();

			for (auto& [key, value] : view)
			{
				nlohmann::json pair;
				pair["key"] = ToJsonRecursively(key, view.get_key_type());
				pair["value"] = ToJsonRecursively(value, view.get_value_type());
				j.push_back(std::move(pair));
			}
		}
	}
	else
	{
		ENGINE_ASSERT(objType.is_class());
		j = nlohmann::json::object();

		for (auto prop : objType.get_properties())
		{
			if (prop.get_metadata(engine::registration::meta::NonSerializable::MetaId).is_valid())
			{
				continue;
			}

			auto value = prop.get_value(objUnwrapped);
			if (!value)
			{
				continue;
			}

			const auto propType = rttr::get_type_or_wrapped_type(prop);
			j[std::string(prop.get_name())] = ToJsonRecursively(value, propType);
		}
	}

	if (!expectedType.is_valid() || rttr::try_get_wrapped_type(expectedType) == rttr::type::get<rttr::variant>())
	{
		WrapIntoObjectIfNeeded(j);

		ENGINE_ASSERT(!objType.get_name().empty());
		j["__type__"] = objType.get_name();
	}
	else
	{
		ENGINE_ASSERT(rttr::try_get_wrapped_type(expectedType) == objType);
	}

	return j;
}
} // unnamed

namespace engine
{

std::string ToJsonString(const rttr::variant& obj, rttr::type expectedType)
{
    PROFILER_CPU_ZONE;

    return ToJsonObject(obj, expectedType).dump(4, ' ');
}

nlohmann::json ToJsonObject(const rttr::variant& obj, rttr::type expectedType)
{
    PROFILER_CPU_ZONE;

    auto json = ToJsonRecursively(obj, expectedType);
    return json;
}

} // engine