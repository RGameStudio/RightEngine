#include <Engine/Tests/Service/DoctestService.hpp>
#include <Engine/Service/WorldService.hpp>
#include <Engine/Engine.hpp>
#include <Engine/System/RenderSystem.hpp>
#include <Engine/Serialization/ToJson.hpp>
#include <Engine/Serialization/FromJson.hpp>
#include <glm/detail/type_quat.hpp>

namespace
{

struct SimpleTestStruct
{
    int kek = 0;
    float kek1 = 0.0;
    std::string kek2;

    bool operator==(const SimpleTestStruct& other) const
    {
        return kek == other.kek && std::abs(kek1 - other.kek1) < std::numeric_limits<float>::epsilon()  && kek2 == other.kek2;
    }

    bool operator<(const SimpleTestStruct& other) const
    {
        return kek < other.kek;
    }
};

SimpleTestStruct CreateNonDefaultSimpleTestStruct()
{
    SimpleTestStruct testStruct;
    testStruct.kek = 228;
    testStruct.kek1 = 1488.1488;
    testStruct.kek2 = "kek2";

    return testStruct;
}

struct ComplexTestStruct
{
    struct NestedStruct
    {
        std::string str;
    };

    SimpleTestStruct kek;
    std::string kek2;
    engine::io::fs::path path;
    NestedStruct nestedStruct;

    glm::vec2 vec2;
    glm::vec3 vec3;
    glm::vec4 vec4;
    glm::quat quat;
    uuids::uuid uuid;
};

struct StructWithCollections
{
    eastl::vector<SimpleTestStruct> vec;
    eastl::unordered_map<std::string, SimpleTestStruct> unordMap;
};

} // unnamed

RTTR_REGISTRATION
{
    using namespace engine::registration;

    Class<SimpleTestStruct>("SimpleTestStruct")
        .Property("kek", &SimpleTestStruct::kek)
        .Property("kek1", &SimpleTestStruct::kek1)
        .Property("kek2", &SimpleTestStruct::kek2);

    Class<ComplexTestStruct>("ComplexTestStruct")
        .Property("kek", &ComplexTestStruct::kek)
        .Property("kek2", &ComplexTestStruct::kek2)
        .Property("path", &ComplexTestStruct::path)
        .Property("nestedStruct",
            [](const ComplexTestStruct& obj)
            {
                return obj.nestedStruct.str;
            },
            [](ComplexTestStruct& obj, std::string str)
            {
                obj.nestedStruct.str = str;
            })
        .Property("vec2", &ComplexTestStruct::vec2)
        .Property("vec3", &ComplexTestStruct::vec3)
        .Property("vec4", &ComplexTestStruct::vec4)
        .Property("quat", &ComplexTestStruct::quat)
        .Property("uuid", &ComplexTestStruct::uuid);

    Class<StructWithCollections>("StructWithCollections")
        .Property("vec", &StructWithCollections::vec)
        .Property("unordMap", &StructWithCollections::unordMap);
        
}

TEST_CASE("Simple json serialization")
{
    SimpleTestStruct testStruct = CreateNonDefaultSimpleTestStruct();

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["kek"], 228);
        CHECK_EQ(json["kek1"], doctest::Approx(1488.1488));
        CHECK_EQ(json["kek2"], "kek2");
    }

    SUBCASE("Deserialize")
    {
        const auto jsonStr = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<SimpleTestStruct>(jsonStr);

        REQUIRE(deserializedStructOpt.has_value());

        const auto v = deserializedStructOpt.value();

        CHECK_EQ(v.kek, 228);
        CHECK_EQ(v.kek1, doctest::Approx(1488.1488));
        CHECK_EQ(v.kek2, "kek2");
    }
}

TEST_CASE("Complex json serialization")
{
    ComplexTestStruct testStruct;
    testStruct.kek2 = "kek2";
    testStruct.kek = CreateNonDefaultSimpleTestStruct();

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["kek"]["kek"], 228);
        CHECK_EQ(json["kek"]["kek1"], doctest::Approx(1488.1488));
        CHECK_EQ(json["kek"]["kek2"], "kek2");
        CHECK_EQ(json["kek2"], "kek2");
    }

    SUBCASE("Deserialize")
    {
        const auto jsonStr = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<ComplexTestStruct>(jsonStr);

        REQUIRE(deserializedStructOpt.has_value());

        const auto v = deserializedStructOpt.value();

        CHECK_EQ(v.kek, CreateNonDefaultSimpleTestStruct());
        CHECK_EQ(v.kek2, "kek2");
    }
}

TEST_CASE("Collection json serialization")
{
    constexpr int C_COLLECTIONS_SIZE = 5;
    StructWithCollections collections;

    for (int i = 0; i < C_COLLECTIONS_SIZE; i++)
    {
        SimpleTestStruct obj = CreateNonDefaultSimpleTestStruct();
        obj.kek = i;
        obj.kek2 = "kek" + std::to_string(i);

        collections.vec.push_back(obj);
        collections.unordMap[std::to_string(i)] = obj;
    }

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(collections);
        REQUIRE(json["unordMap"].is_object());

        auto& map = json["unordMap"];
        auto& vec = json["vec"];
        REQUIRE(vec.is_array());

        for (int i = 0; i < C_COLLECTIONS_SIZE; i++)
        {
            auto& obj = map[std::to_string(i)];
            REQUIRE(obj.is_object());

            CHECK_EQ(obj["kek"], i);
            CHECK_EQ(obj["kek1"], doctest::Approx(1488.1488));
            CHECK_EQ(obj["kek2"], "kek" + std::to_string(i));
        }

        int i = 0;
        for (auto& obj : vec)
        {
            REQUIRE(obj.is_object());

            CHECK_EQ(obj["kek"], i);
            CHECK_EQ(obj["kek1"], doctest::Approx(1488.1488));
            CHECK_EQ(obj["kek2"], "kek" + std::to_string(i));
            i++;
        }
    }

    SUBCASE("Deserialize")
    {
        const auto json = engine::ToJsonString(collections);
        const auto deserializedStructOpt = engine::FromJsonString<StructWithCollections>(json);

        REQUIRE(deserializedStructOpt.has_value());

        const auto collectionsObj = deserializedStructOpt.value();
        REQUIRE(collectionsObj.unordMap.size() == C_COLLECTIONS_SIZE);
        REQUIRE(collectionsObj.vec.size() == C_COLLECTIONS_SIZE);

        for (int i = 0; i < C_COLLECTIONS_SIZE; i++)
        {
            auto mapIt = collectionsObj.unordMap.find(std::to_string(i));
            REQUIRE(mapIt != collectionsObj.unordMap.end());

            auto& vecIt = collectionsObj.vec[i];

            CHECK_EQ(mapIt->second.kek, i);
            CHECK_EQ(mapIt->second.kek1, doctest::Approx(1488.1488));
            CHECK_EQ(mapIt->second.kek2, "kek" + std::to_string(i));

            CHECK_EQ(vecIt.kek, i);
            CHECK_EQ(vecIt.kek1, doctest::Approx(1488.1488));
            CHECK_EQ(vecIt.kek2, "kek" + std::to_string(i));
        }
    }
}

TEST_CASE("Path json serialization test")
{
    constexpr std::string_view C_PATH = "/Engine/Test/Path";
    ComplexTestStruct testStruct;
    testStruct.path = C_PATH;

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["path"], C_PATH.data());
    }

    SUBCASE("Deserialize")
    {
        const auto json = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<ComplexTestStruct>(json);

        REQUIRE(deserializedStructOpt.has_value());

        CHECK_EQ(deserializedStructOpt.value().path.generic_string(), C_PATH);
    }
}

TEST_CASE("Custom getter and setter for property")
{
    constexpr std::string_view C_TEST_STR = "nestedStruct";
    ComplexTestStruct testStruct;
    testStruct.nestedStruct.str = C_TEST_STR;

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["nestedStruct"], C_TEST_STR.data());
    }

    SUBCASE("Deserialize")
    {
        const auto json = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<ComplexTestStruct>(json);

        REQUIRE(deserializedStructOpt.has_value());
        CHECK_EQ(deserializedStructOpt.value().nestedStruct.str, C_TEST_STR);
    }
}

TEST_CASE("GLM serialization")
{
    constexpr glm::vec2 C_VEC2 = { 1, 2 };
    constexpr glm::vec3 C_VEC3 = { 1, 28, 3 };
    constexpr glm::vec4 C_VEC4 = { 11, 2, 3, 4 };
    constexpr glm::quat C_QUAT = { 1, 22, 3, 5 };

    ComplexTestStruct testStruct;
    testStruct.vec2 = C_VEC2;
    testStruct.vec3 = C_VEC3;
    testStruct.vec4 = C_VEC4;
    testStruct.quat = C_QUAT;

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["vec2"]["x"], C_VEC2.x);
        CHECK_EQ(json["vec2"]["y"], C_VEC2.y);

        CHECK_EQ(json["vec3"]["x"], C_VEC3.x);
        CHECK_EQ(json["vec3"]["y"], C_VEC3.y);
        CHECK_EQ(json["vec3"]["z"], C_VEC3.z);

        CHECK_EQ(json["vec4"]["x"], C_VEC4.x);
        CHECK_EQ(json["vec4"]["y"], C_VEC4.y);
        CHECK_EQ(json["vec4"]["z"], C_VEC4.z);
        CHECK_EQ(json["vec4"]["w"], C_VEC4.w);

        CHECK_EQ(json["quat"]["x"], C_QUAT.x);
        CHECK_EQ(json["quat"]["y"], C_QUAT.y);
        CHECK_EQ(json["quat"]["z"], C_QUAT.z);
        CHECK_EQ(json["quat"]["w"], C_QUAT.w);
    }

    SUBCASE("Deserialize")
    {
        const auto json = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<ComplexTestStruct>(json);

        REQUIRE(deserializedStructOpt.has_value());

        const auto obj = deserializedStructOpt.value();

        CHECK_EQ(obj.vec2, C_VEC2);
        CHECK_EQ(obj.vec3, C_VEC3);
        CHECK_EQ(obj.vec4, C_VEC4);
        CHECK_EQ(obj.quat, C_QUAT);
    }
}

TEST_CASE("UUID serialization")
{
    constexpr std::string_view C_UUID = "c6cb9bc2-3e6c-48c3-bbcb-e5a67040b9bc";

    ComplexTestStruct testStruct;
    testStruct.uuid = uuids::uuid::from_string(C_UUID).value();

    SUBCASE("Serialize")
    {
        const auto json = engine::ToJsonObject(testStruct);

        CHECK_EQ(json["uuid"], C_UUID.data());
    }

    SUBCASE("Deserialize")
    {
        const auto json = engine::ToJsonString(testStruct);
        const auto deserializedStructOpt = engine::FromJsonString<ComplexTestStruct>(json);

        REQUIRE(deserializedStructOpt.has_value());

        const auto obj = deserializedStructOpt.value();

        CHECK_EQ(obj.uuid, uuids::uuid::from_string(C_UUID).value());
    }
}