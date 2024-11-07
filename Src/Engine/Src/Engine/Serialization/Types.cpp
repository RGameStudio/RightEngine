#include <Engine/Registration.hpp>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <uuid.h>

RTTR_REGISTRATION
{
	using namespace engine;

    registration::Class<glm::vec2>("glm::vec2")
        .Property<float>("x", &glm::vec2::x)
        .Property<float>("y", &glm::vec2::y);

    registration::Class<glm::vec3>("glm::vec3")
        .Property<float>("x", &glm::vec3::x)
        .Property<float>("y", &glm::vec3::y)
        .Property<float>("z", &glm::vec3::z);

    registration::Class<glm::vec4>("glm::vec4")
        .Property<float>("x", &glm::vec4::x)
        .Property<float>("y", &glm::vec4::y)
        .Property<float>("z", &glm::vec4::z)
        .Property<float>("w", &glm::vec4::w);

    registration::Class<glm::quat>("glm::quat")
        .Property<float>("x", &glm::quat::x)
        .Property<float>("y", &glm::quat::y)
        .Property<float>("z", &glm::quat::z)
        .Property<float>("w", &glm::quat::w);

    registration::Class<uuids::uuid>("uuids::uuid")
        .Meta(registration::meta::C_METAINFO_IS_STRING_LIKE_TAG, true);

    rttr::type::register_converter_func([](const uuids::uuid& uuid, bool& ok) -> std::string 
    {
        ok = true;
        return uuids::to_string(uuid);
    });

    rttr::type::register_converter_func([](const std::string& str, bool& ok) -> uuids::uuid 
    {
        ok = true;
        const auto res = uuids::uuid::from_string(str);
        if (res.has_value())
        {
            return res.value();
        }

        ENGINE_ASSERT_WITH_MESSAGE(false, fmt::format("Invalid uuid: '{}'", str));
        return uuids::uuid();
    });

    rttr::type::register_converter_func([](std::string_view str, bool& ok) -> uuids::uuid
    {
        ok = true;
        const auto res = uuids::uuid::from_string(str);
        if (res.has_value())
        {
            return res.value();
        }

        ENGINE_ASSERT_WITH_MESSAGE(false, fmt::format("Invalid uuid: '{}'", str));
        return uuids::uuid();
    });
}
