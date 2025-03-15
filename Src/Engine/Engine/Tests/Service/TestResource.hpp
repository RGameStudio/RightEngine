#pragma once

#include <Engine/Service/Resource/Resource.hpp>
#include <Engine/Service/Resource/Loader.hpp>

namespace tests
{

class TestResource;

class ENGINE_API TestResourceLoader final : public engine::Loader
{
public:
	TestResourceLoader();

	virtual void				Update() override;

	virtual engine::ResPtr<engine::IResource>	Load(const engine::io::fs::path& path) override;

	virtual engine::ResPtr<engine::IResource>	Get(const engine::io::fs::path& path) const override;

	virtual void				LoadSystemResources() override {}

private:
	bool Load(const engine::ResPtr<TestResource>& resource);

	std::mutex																	m_mutex;
	eastl::vector<tf::Future<void>>												m_loadingTasks;
	eastl::unordered_map<engine::io::fs::path, engine::ResPtr<TestResource>>	m_cache;
};

class ENGINE_API TestResource final : public engine::Resource<TestResource>
{
public:
	TestResource(const engine::io::fs::path& path);
    virtual ~TestResource() override {}

	friend class TestResourceLoader;
};

} // tests