#include <Engine/Service/Resource/SceneResource.hpp>
#include <Engine/Service/Filesystem/File.hpp>
#include <Engine/Serialization/FromJson.hpp>
#include <Engine/ECS/World.hpp>
#include <Engine/System/TransformSystem.hpp>
#include <Engine/System/SkyboxSystem.hpp>
#include <Engine/System/RenderSystem.hpp>
#include <simdjson.h>

#define UUID_SYSTEM_GENERATOR
#include <uuid.h>

RTTR_REGISTRATION
{
	using namespace engine::registration;

    ResourceLoader<engine::SceneLoader>("engine::SceneLoader");
}

namespace engine
{
SceneLoader::SceneLoader()
{
}

void SceneLoader::Update()
{
	PROFILER_CPU_ZONE;
}

ResPtr<Resource> SceneLoader::Load(const fs::path& path)
{
	std::lock_guard l(m_mutex);

	if (auto res = Get(path))
	{
		return res;
	}

	auto resource = MakeResPtr<SceneResource>(path);
	resource->m_status = Resource::Status::LOADING;
	m_cache[path] = resource;

	auto& ts = Instance().Service<ThreadService>();

	ts.AddBackgroundTask([this, resource]()
		{
			PROFILER_CPU_ZONE_NAME("Load scene");
			const auto result = Load(resource);
			resource->m_status = result ? Resource::Status::READY : Resource::Status::FAILED;
		});

	return resource;
}

ResPtr<Resource> SceneLoader::Get(const fs::path& path) const
{
	if (const auto it = m_cache.find(path); it != m_cache.end())
	{
		return it->second;
	}
	return {};
}

bool SceneLoader::Load(const ResPtr<SceneResource>& resource)
{
	using namespace simdjson;

	io::File worldFile(resource->m_srcPath);
	if (!worldFile.Read(false))
	{
		return false;
	}

	dom::parser parser;
	dom::element j;
	const auto jsonStr = worldFile.AsStr();

	auto error = parser.parse(jsonStr.data(), worldFile.Size(), false).get(j);
	if (error)
	{
	    core::log::error("[SceneResource] Json parse error: {}", error_message(error));
		return false;
	}

	auto world = std::make_shared<ecs::World>(j["name"]);
	resource->m_world = world;

	if (const auto& entities = j.at_key("entities").get_array(); !entities.error())
	{
		for (auto entityJson : entities.value_unsafe())
		{
			dom::object itemObj = entityJson.get_object();
			std::string name = std::string(itemObj["name"]);
			std::string uuidStr = std::string(itemObj["uuid"]);
			uuids::uuid uuid;
			if (const auto opt = uuids::uuid::from_string(uuidStr); opt.has_value())
			{
				uuid = opt.value();
			}
			else
			{
				uuid = uuids::uuid_system_generator{}();
			}

			world->m_entityManager->CreateEntityForce(name, uuid);

			for (auto compJson : itemObj.at_key("components").get_array().value_unsafe())
			{
				for (const auto& [type, value] : compJson.get_object())
				{
					rttr::type t = rttr::type::get_by_name(type);
					if (!t.is_valid() || !registration::helpers::typeRegistered(t))
					{
						core::log::error("[SceneResource] Unknown component type '{}'", t.get_name());
						continue;
					}

					auto component = FromJsonObject(value, t);
					if (!component.is_valid())
					{
						core::log::error("[SceneResource] Error in deserializing component '{}'", t.get_name());
						continue;
					}

					if (t == rttr::type::get<TransformComponent>())
					{
						world->m_entityManager->AddComponent<TransformComponent>(uuid, std::move(component.get_value_unsafe<TransformComponent>()));
					}
					else if (t == rttr::type::get<SkyboxComponent>())
					{
						world->m_entityManager->AddComponent<SkyboxComponent>(uuid, std::move(component.get_value_unsafe<SkyboxComponent>()));
					}
					else if (t == rttr::type::get<MeshComponent>())
					{
						world->m_entityManager->AddComponent<MeshComponent>(uuid, std::move(component.get_value_unsafe<MeshComponent>()));
					}
					else if (t == rttr::type::get<CameraComponent>())
					{
						world->m_entityManager->AddComponent<CameraComponent>(uuid, std::move(component.get_value_unsafe<CameraComponent>()));
					}
				}
			}
		}
	}

	return true;
}

SceneResource::SceneResource(const io::fs::path& path) : Resource(path)
{
}

}
