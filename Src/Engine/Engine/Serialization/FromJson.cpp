#include <Engine/Serialization/FromJson.hpp>
#include <taskflow/algorithm/for_each.hpp>

namespace
{

constexpr const size_t C_MIN_ARRAY_ITEMS_TO_PARALLELIZE = std::numeric_limits<size_t>::max();

rttr::type ExtractType(const simdjson::dom::element& j)
{
	switch (j.type())
	{
	case simdjson::dom::element_type::DOUBLE: return rttr::type::get<double>();
	case simdjson::dom::element_type::INT64: return rttr::type::get<int64_t>();
	case simdjson::dom::element_type::UINT64: return rttr::type::get<uint64_t>();
	case simdjson::dom::element_type::BOOL: return rttr::type::get<bool>();
	case simdjson::dom::element_type::STRING: return rttr::type::get<std::string>();
	case simdjson::dom::element_type::OBJECT:
	{
		std::string_view typeName;
		if (j.at_key("__type__").get_string().get(typeName) != simdjson::SUCCESS)
		{
			break;
		}

		auto type = rttr::type::get_by_name(typeName);
		if (!type)
		{
			if ((engine::Instance().Cfg().m_domain & engine::Domain::EDITOR) == engine::Domain::EDITOR)
			{
				core::log::warning("Could not deserialize unknown type <{}>", typeName);
			}
		}
		return type;
	}
	default: 
		break;
	}

	core::log::error("Failed to extract from JSON type {}.", eastl::to_underlying(j.type()));
	return {};
}

template<typename T>
rttr::variant ExtractNumericType(T val, rttr::type t)
{
	if (t == rttr::type::get<bool>())			return static_cast<bool>(val);
	else if (t == rttr::type::get<char>())		return static_cast<char>(val);
	else if (t == rttr::type::get<int8_t>())	return static_cast<int8_t>(val);
	else if (t == rttr::type::get<int16_t>())	return static_cast<int16_t>(val);
	else if (t == rttr::type::get<int32_t>())	return static_cast<int32_t>(val);
	else if (t == rttr::type::get<int64_t>())	return static_cast<int64_t>(val);
	else if (t == rttr::type::get<uint8_t>())	return static_cast<uint8_t>(val);
	else if (t == rttr::type::get<uint16_t>())	return static_cast<uint16_t>(val);
	else if (t == rttr::type::get<uint32_t>())	return static_cast<uint32_t>(val);
	else if (t == rttr::type::get<uint64_t>())	return static_cast<uint64_t>(val);
	else if (t == rttr::type::get<float>())		return static_cast<float>(val);
	else if (t == rttr::type::get<double>())	return static_cast<double>(val);

	core::log::error("Could not extract unknown numeric type <{}>", t.get_name());
	return val;
}

rttr::variant ExtractFromString(std::string_view val, const rttr::type type)
{
	rttr::variant retval = std::string(val);
	if (retval.get_type() == type)
	{
		return retval;
	}

	if (type == rttr::type::get<std::string_view>())
	{
		return retval;
	}

	bool success = retval.convert(type);
	if (!success)
	{
		core::log::warning("Failed to deserialize string '{}' to type {}", val, type.get_name());
	}

	return retval;
}

rttr::variant ConstructObject(const rttr::type t)
{
	auto constructor = t.get_constructor();
	if (!constructor.is_valid())
	{
		return {};
	}

	auto obj = constructor.invoke();
	ENGINE_ASSERT(obj.is_valid());
	return obj;
}

void FromJsonRecursively(simdjson::dom::element j, rttr::variant& obj, const rttr::type t, bool allowParallelize);

rttr::variant FromJsonRecursively(const simdjson::dom::element& j, const rttr::type t, bool allowParallelize)
{
	rttr::variant res;
	FromJsonRecursively(j, res, t, allowParallelize);
	return res;
}

void ExtractFromArray(const simdjson::dom::array& j, const rttr::variant& obj, bool allowParallelize)
{
	if (const auto type = rttr::get_type_or_wrapped_type(obj); type.is_sequential_container())
	{
		auto view = obj.create_sequential_view();
		auto valueType = view.get_value_type();

		if (!view.set_size(j.size()) && view.get_size() != j.size())
		{
			core::log::error("Failed to resize array of type <{}> to {}; size is {}.", type.get_name(), j.size(), view.get_size());
		}

		if (j.size() > C_MIN_ARRAY_ITEMS_TO_PARALLELIZE && allowParallelize)
		{
			eastl::vector<simdjson::dom::element> tmp;
			tmp.reserve(j.size());
			for (auto elem : j)
			{
				tmp.push_back(elem);
			}

			tf::Taskflow taskFlow;
			taskFlow.for_each(tmp.begin(), tmp.end(), [&tmp, &view, &valueType](auto&& range)
				{
					for (const auto& elem : range)
					{
						auto val = FromJsonRecursively(elem, valueType, false);
						const auto i = &elem - tmp.data();
						if (!view.set_value_move(i, val))
						{
							core::log::error("Failed to set array element #{} of type <{}>", i, valueType.get_name());
						}
					}
				});

			auto& ts = engine::Instance().Service<engine::ThreadService>();
			ts.AddBackgroundTaskflow(std::move(taskFlow)).wait();
		}
		else
		{
			size_t i = 0;

			for (auto elem : j)
			{
				auto val = FromJsonRecursively(elem, valueType, allowParallelize);
				if (!view.set_value_move(i++, val))
				{
					core::log::error("Failed to set array element #{} of type <{}>", i, valueType.get_name());
				}
			}
		}
	}
	else if (type.is_associative_container())
	{
		auto view = obj.create_associative_view();
		view.clear();
		auto keyType = view.get_key_type();
		if (view.is_key_only_type())
		{
			for (auto elem : j)
			{
				auto key = FromJsonRecursively(elem, keyType, allowParallelize);
				if (view.insert_move(key).first == view.end())
				{
					core::log::error("Failed to insert element of type <{}>", keyType.get_name());
				}
			}
		}
		else
		{
			auto valType = view.get_value_type();
			for (auto elem : j)
			{
				simdjson::dom::element jKey, jVal;
				if (elem.at_key("key").get(jKey) != simdjson::SUCCESS ||
					elem.at_key("value").get(jVal) != simdjson::SUCCESS)
				{
					core::log::error("Failed to deserialize element of key-value array to type {}", obj.get_type().get_name());
					continue;
				}
				auto key = FromJsonRecursively(jKey, keyType, false);
				auto val = FromJsonRecursively(jVal, valType, allowParallelize);
				if (!view.insert_move(key, val).second)
				{
					core::log::error("Failed to insert element of type <{}> -> <{}>", keyType.get_name(), valType.get_name());
				}
			}
		}
	}
	else
	{
		core::log::error("Failed to deserialize JSON array to type {}", obj.get_type().get_name());
	}
}

void ExtractFromObject(simdjson::dom::object j, const rttr::variant& obj, bool allowParallelize)
{
	if (const auto type = rttr::get_type_or_wrapped_type(obj); type.is_associative_container())
	{
		auto view = obj.create_associative_view();
		view.clear(); //-- we must clear all before inserting elements cause of view can be not empty

		auto keyType = view.get_key_type();
		auto valType = view.get_value_type();
		for (auto [jKey, jVal] : j)
		{
			if (jKey == "__type__")
			{
				continue;
			}

			auto key = ExtractFromString(jKey, keyType);
			auto val = FromJsonRecursively(jVal, valType, allowParallelize);
			if (!view.insert_move(key, val).second)
			{
				core::log::error("Failed to insert element of type <{}> -> <{}>", keyType.get_name(), valType.get_name());
			}
		}
	}
	else if (type.is_class() || (type.is_pointer() && type.get_raw_type().is_class()))
	{
		const auto objProps = rttr::get_type_or_wrapped_type(obj).get_properties();

		for (auto prop : objProps)
		{
			simdjson::dom::element jVal;
			if (j.at_key(prop.get_name()).get(jVal) != simdjson::SUCCESS)
			{
				continue;
			}

			const auto propType = rttr::get_type_or_wrapped_type(prop);
			auto value = propType.create();

			if (propType.is_array())
			{
				value = prop.get_value(obj);
			}

			FromJsonRecursively(jVal, value, propType, allowParallelize);
			if (!prop.set_value_move(obj, value))
			{
				core::log::warning("Failed to set property {}::{}", type.get_name(), prop.get_name());
			}
		}
	}
	else
	{
		core::log::error("Failed to deserialize JSON object to type {}", type.get_name());
	}
}

void FromJsonRecursively(simdjson::dom::element j, rttr::variant& obj, rttr::type type, bool allowParallelize)
{
	if (j.is_null())
	{
		obj = {};
		return;
	}

	if (!obj && (!type.is_valid() || type == rttr::type::get<rttr::variant>()))
	{
		auto extType = ExtractType(j);
		if (!extType)
		{
			return;
		}
		type = extType;
	}

	//-- unwrap non-object json element with metadata, if needed
	simdjson::dom::element jVal;
	if (j.at_key("__value__").get(jVal) == simdjson::SUCCESS)
	{
		j = jVal;
	}

	auto checkValue = [](auto&& result)
	{
		ENGINE_ASSERT_WITH_MESSAGE(result.error() == simdjson::SUCCESS, fmt::format("Error while serializing from json: '{}'", simdjson::error_message(result.error())));

		return result.value_unsafe();
	};

	//-- try deserializing json into requested type
	switch (j.type())
	{
	case simdjson::dom::element_type::DOUBLE:
		obj = ExtractNumericType(checkValue(j.get<double>()), type);
		break;
	case simdjson::dom::element_type::INT64:
		obj = ExtractNumericType(checkValue(j.get<int64_t>()), type);
		break;
	case simdjson::dom::element_type::UINT64:
		obj = ExtractNumericType(checkValue(j.get<uint64_t>()), type);
		break;
	case simdjson::dom::element_type::BOOL:
		obj = ExtractNumericType(checkValue(j.get<bool>()), type);
		break;
	case simdjson::dom::element_type::STRING:
		obj = ExtractFromString(checkValue(j.get<std::string_view>()), type);
		break;
	case simdjson::dom::element_type::ARRAY:
		if (!obj)
		{
			obj = ConstructObject(type);
		}
		ExtractFromArray(checkValue(j.get<simdjson::dom::array>()), obj, allowParallelize);
		break;
	case simdjson::dom::element_type::OBJECT:
		if (!obj)
		{
			obj = ConstructObject(type);
		}
		ExtractFromObject(checkValue(j.get<simdjson::dom::object>()), obj, allowParallelize);
		break;
	default:
		ENGINE_ASSERT(false);
	}
}
    
} // unnamed

namespace engine
{

rttr::variant FromJsonObject(const simdjson::dom::element& j, rttr::type expectedType)
{
	PROFILER_CPU_ZONE;
	return FromJsonRecursively(j, expectedType ? expectedType : rttr::try_get_wrapped_type(expectedType), true);
}

rttr::variant FromJsonString(const std::string& str, rttr::type expectedType, std::string* errorBuffer)
{
	PROFILER_CPU_ZONE;

	simdjson::dom::parser parser;
	simdjson::dom::element json;

	bool needRealloc = (str.capacity() - str.size()) < simdjson::SIMDJSON_PADDING;

	auto error = parser.parse(str.data(), str.size(), needRealloc).get(json);
	if (error)
	{
		if (errorBuffer)
		{
			*errorBuffer = simdjson::error_message(error);
		}

		return rttr::variant();
	}

	return FromJsonObject(json, expectedType);
}
}