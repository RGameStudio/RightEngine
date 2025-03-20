#include <Engine/Tests/Service/TestResource.hpp>

namespace tests
{

TestResourceLoader::TestResourceLoader()
{
}

void TestResourceLoader::Update()
{
	PROFILER_CPU_ZONE;
}

engine::ResPtr<engine::IResource> TestResourceLoader::Load(const engine::io::fs::path& path)
{
	std::lock_guard l(m_mutex);

	if (auto res = Get(path))
	{
		return res;
	}

	auto resource = engine::MakeResPtr<TestResource>(path);
	resource->m_status = engine::IResource::Status::LOADING;
	m_cache[path] = resource;

	auto& ts = engine::Instance().Service<engine::ThreadService>();

	ts.AddBackgroundTask([this, resource]()
		{
			PROFILER_CPU_ZONE_NAME("Load test");
			const auto result = Load(resource);
			resource->m_status = result ? engine::IResource::Status::READY : engine::IResource::Status::FAILED;
		});

	return resource;
}

engine::ResPtr<engine::IResource> TestResourceLoader::Get(const engine::io::fs::path& path) const
{
	if (const auto it = m_cache.find(path); it != m_cache.end())
	{
		return it->second;
	}
	return {};
}

bool TestResourceLoader::Load(const engine::ResPtr<TestResource>& resource)
{
	return true;
}

TestResource::TestResource(const engine::io::fs::path& path) : Resource(path)
{
}

} // tests