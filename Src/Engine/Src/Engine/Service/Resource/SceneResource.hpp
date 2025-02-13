#pragma once

#include <Engine/Service/Resource/Resource.hpp>
#include <Engine/Service/Resource/Loader.hpp>
#include <taskflow/taskflow.hpp>

namespace engine
{

class SceneResource;

class ENGINE_API SceneLoader : public Loader
{
public:
	SceneLoader();

	virtual void				Update() override;

	virtual ResPtr<Resource>	Load(const fs::path& path) override;

	virtual ResPtr<Resource>	Get(const fs::path& path) const override;

	virtual void				LoadSystemResources() override {}

private:
	bool Load(const ResPtr<SceneResource>& resource);

	std::mutex												m_mutex;
	eastl::vector<tf::Future<void>>							m_loadingTasks;
	eastl::unordered_map<fs::path, ResPtr<SceneResource>>	m_cache;
};

class ENGINE_API SceneResource final : public Resource
{
public:
	SceneResource(const io::fs::path& path);
	virtual ~SceneResource() {}

	const ResPtr<ecs::World>& World() const { return m_world; }

	friend class SceneLoader;

private:
	ResPtr<ecs::World> m_world;
};


} //engine