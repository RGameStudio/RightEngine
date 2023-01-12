#include "SceneSerializer.hpp"
#include "Path.hpp"
#include "Entity.hpp"
#include "AssetManager.hpp"
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
    void SerializeList(YAML::Emitter& output, const std::string& listName, std::function<void()> listFn)
    {
        output << YAML::BeginMap;
        output << YAML::Key << listName << YAML::Value << YAML::BeginSeq;
        listFn();
        output << YAML::EndSeq;
        output << YAML::EndMap;
    }

    template<typename T>
    void SerializeKeyValue(YAML::Emitter& output, const std::string& key, const T& value)
    {
        output << YAML::BeginMap;
        output << YAML::Key << key << YAML::Value << value;
        output << YAML::EndMap;
    }

    template<typename T>
    void SerializeComponent(YAML::Emitter& output,
                            const std::shared_ptr<Entity>& entity,
                            const std::string& componentName,
                            std::function<void(const T&)> componentCallback)
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

    std::string GetAssetTypeString(AssetType type)
    {
        switch (type)
        {
            case AssetType::IMAGE:
                return "Image";
            case AssetType::MESH:
                return "Mesh";
            case AssetType::ENVIRONMENT_MAP:
                return "Environment Map";
            case AssetType::SHADER:
                return "Shader";
            case AssetType::MATERIAL:
                return "Material";
            default:
            R_CORE_ASSERT(false, "")
        }
    }

    void SerializeAsset(YAML::Emitter& output, const xg::Guid& guid)
    {
        const auto assetPtr = AssetManager::Get().GetAsset<AssetBase>({ guid });
        R_CORE_ASSERT(assetPtr, "");

        output << YAML::BeginMap;
        output << YAML::Key << GetAssetTypeString(assetPtr->type) << YAML::Value << YAML::BeginSeq;
        SerializeKeyValue(output, "GUID", assetPtr->guid);
        SerializeKeyValue(output, "Path", assetPtr->path);
        if (assetPtr->type == AssetType::MATERIAL)
        {
            const auto materialPtr = AssetManager::Get().GetAsset<Material>({ guid });

            SerializeList(output, "Texture Data", [&]()
            {
                SerializeKeyValue(output, "Albedo GUID", materialPtr->textureData.albedo.guid);
                SerializeKeyValue(output, "Normal GUID", materialPtr->textureData.normal.guid);
                SerializeKeyValue(output, "Roughness GUID", materialPtr->textureData.roughness.guid);
                SerializeKeyValue(output, "Metallic GUID", materialPtr->textureData.metallic.guid);
                SerializeKeyValue(output, "AO GUID", materialPtr->textureData.ao.guid);
            });

            SerializeList(output, "Material Data", [&]()
            {
                SerializeKeyValue(output, "Albedo", materialPtr->materialData.albedo);
                SerializeKeyValue(output, "Roughness", materialPtr->materialData.roughness);
                SerializeKeyValue(output, "Metallic", materialPtr->materialData.metallic);
            });
        }
        output << YAML::EndSeq;
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
    output << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;
    SerializeAssets(output);
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
        sceneAssets.insert(component.GetMesh().guid);
        SaveMaterial(component.GetMaterial());
    });

    SerializeComponent<LightComponent>(output, entity, "Light component", [&](const auto& component)
    {
        SerializeKeyValue(output, "Type", static_cast<int>(component.type));
        SerializeKeyValue(output, "Color", component.color);
        SerializeKeyValue(output, "Intensity", component.intensity);
    });

    SerializeComponent<SkyboxComponent>(output, entity, "Skybox component", [&](const auto& component)
    {
        SerializeKeyValue(output, "Type", static_cast<int>(component.type));
        SerializeKeyValue(output, "Skybox GUID", component.environmentHandle.guid);
        sceneAssets.insert(component.environmentHandle.guid);
    });

    SerializeComponent<CameraComponent>(output, entity, "Camera component", [&](const auto& component)
    {
        SerializeKeyValue(output, "Front", component.front);
        SerializeKeyValue(output, "World up", component.worldUp);
        SerializeKeyValue(output, "Up", component.up);
        SerializeKeyValue(output, "Z near", component.zNear);
        SerializeKeyValue(output, "Z far", component.zFar);
        SerializeKeyValue(output, "Aspect ratio", component.aspectRatio);
        SerializeKeyValue(output, "FOV", component.GetFOV());
        SerializeKeyValue(output, "Movement speed", component.movementSpeed);
        SerializeKeyValue(output, "Sensitivity", component.sensitivity);
        SerializeKeyValue(output, "Active", component.isActive);
        SerializeKeyValue(output, "Primary", component.isPrimary);
    });

    output << YAML::EndSeq;
    output << YAML::EndMap;

    for (const auto& child : entity->GetChildren())
    {
        SerializeEntity(output, child);
    }
}

void SceneSerializer::SerializeAssets(YAML::Emitter& output)
{
    for (auto& guid : sceneAssets)
    {
        SerializeAsset(output, guid);
    }
}

void SceneSerializer::SaveMaterial(const AssetHandle& handle)
{
    const auto assetPtr = AssetManager::Get().GetAsset<Material>(handle);
    R_CORE_ASSERT(assetPtr, "");

    sceneAssets.insert(assetPtr->textureData.albedo.guid);
    sceneAssets.insert(assetPtr->textureData.normal.guid);
    sceneAssets.insert(assetPtr->textureData.roughness.guid);
    sceneAssets.insert(assetPtr->textureData.metallic.guid);
    sceneAssets.insert(assetPtr->textureData.ao.guid);

    sceneAssets.insert(handle.guid);
}