#include "SceneSerializer.hpp"
#include "Path.hpp"
#include "Entity.hpp"
#include <fstream>

using namespace RightEngine;

namespace YAML
{
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<xg::Guid>
    {
        static Node encode(const xg::Guid& uuid)
        {
            Node node;
            node.push_back(uuid.str());
            return node;
        }

        static bool decode(const Node& node, xg::Guid& uuid)
        {
            uuid = xg::Guid(node.as<std::string>());
            return true;
        }
    };

    Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }
}

namespace
{
    template<typename T>
    void SerializeComponent(YAML::Emitter& output, const std::shared_ptr<Entity>& entity, const std::string& componentName, std::function<void(const T&)> componentCallback)
    {
        if (entity->HasComponent<T>())
        {
            output << YAML::BeginMap;
            output << YAML::Key << componentName << YAML::Value << YAML::BeginSeq;
            componentCallback(entity->GetComponent<T>());
            output << YAML::EndSeq;
            output << YAML::EndMap;
        }
    }

    template<typename T>
    void SerializeKeyValue(YAML::Emitter& output, const std::string& key, const T& value)
    {
        output << YAML::BeginMap;
        output << YAML::Key << key << YAML::Value << value;
        output << YAML::EndMap;
    }
}

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : scene(scene)
{
}

bool SceneSerializer::Serialize(const std::string& path)
{
    YAML::Emitter output;
    output << YAML::BeginMap;
    output << YAML::Key << "Scene" << YAML::Value << "Untitled";
    output << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    SerializeEntity(output, scene->GetRootNode());
    output << YAML::EndSeq;
    output << YAML::EndMap;

    auto aPath = Path::ConvertEnginePathToOSPath(path);
    std::ofstream fout(aPath);
    fout << output.c_str();
    R_CORE_INFO("Serialized scene to {} successfully!", Path::ConvertEnginePathToOSPath(path));
    return true;
}

bool SceneSerializer::Deserialize(const std::string& path)
{
    return false;
}

void SceneSerializer::SerializeEntity(YAML::Emitter& output, const std::shared_ptr<Entity>& entity)
{
    R_CORE_ASSERT(entity->HasComponent<TagComponent>(), "");

    output << YAML::BeginMap;
    output << YAML::Key << "Entity";
    output << YAML::BeginSeq;

    SerializeComponent<TagComponent>(output, entity, "Tag component", [&](const auto& component)
    {
        SerializeKeyValue(output, "GUID", component.guid);
        SerializeKeyValue(output, "Name", component.name);
    });

    SerializeComponent<TransformComponent>(output, entity, "Transform component", [&](const auto& component)
    {
        SerializeKeyValue(output, "Position", component.position);
        SerializeKeyValue(output, "Rotation", component.rotation);
        SerializeKeyValue(output, "Scale", component.scale);
    });

    SerializeComponent<MeshComponent>(output, entity, "Mesh component", [&](const auto& component)
    {
        SerializeKeyValue(output, "Mesh GUID", component.GetMesh().guid);
        SerializeKeyValue(output, "Material GUID", component.GetMaterial().guid);
        SerializeKeyValue(output, "Is visible", component.IsVisible());
    });

    output << YAML::EndSeq;
    output << YAML::EndMap;

    for (const auto& child : entity->GetChildren())
    {
        SerializeEntity(output, child);
    }
}
