#pragma once

#include "Scene.hpp"
#include <yaml-cpp/yaml.h>

namespace RightEngine
{
    class SceneSerializer
    {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& aScene);

        bool Serialize(const std::string& path);
        bool Deserialize(const std::string& path);

    private:
        void SerializeEntity(YAML::Emitter& output, const std::shared_ptr<Entity>& entity);

        std::shared_ptr<Scene> scene;
    };
}