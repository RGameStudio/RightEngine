#pragma once

#include <Engine/Service/Resource/Resource.hpp>
#include <Engine/Service/Resource/Loader.hpp>
#include <Engine/Service/Render/Material.hpp>
#include <RHI/Texture.hpp>
#include <taskflow/taskflow.hpp>

namespace engine
{

struct EnvironmentMap
{
	ResPtr<MaterialResource>	m_material;
	RPtr<rhi::Texture>			m_cubemap;
	RPtr<rhi::Texture>			m_irradianceTexture;
	RPtr<rhi::Texture>			m_prefilterTexture;
	RPtr<rhi::Texture>			m_brdfTexture;
};

class EnvironmentMapResource;

class ENGINE_API EnvironmentMapLoader final : public Loader
{
public:
	EnvironmentMapLoader();

	virtual void					Update() override;

	virtual ResPtr<IResource>		Load(const fs::path& path) override;

	virtual ResPtr<IResource>		Get(const fs::path& path) const override;

	virtual void					LoadSystemResources() override;

	const ResPtr<MaterialResource>& SkyboxMaterial() const { return m_skyboxMaterial; }

private:
	bool							Load(const ResPtr<EnvironmentMapResource>& resource);

	mutable std::mutex																	m_mutex;
	eastl::vector<tf::Future<void>>														m_loadingTasks;
	eastl::unordered_map<fs::path, ResPtr<EnvironmentMapResource>>						m_cache;

	ResPtr<MaterialResource>															m_equirectToCubemapMaterial;
	ResPtr<MaterialResource>															m_envmapIrradianceMaterial;
	ResPtr<MaterialResource>															m_envmapPrefilterMaterial;
	ResPtr<MaterialResource>															m_irradianceLoadMaterial;
	ResPtr<MaterialResource>															m_prefilterLoadMaterial;
	ResPtr<MaterialResource>															m_skyboxMaterial;
};

class ENGINE_API EnvironmentMapResource final : public Resource<EnvironmentMapResource>
{
public:
	EnvironmentMapResource(const io::fs::path& path);
	virtual ~EnvironmentMapResource() {}

	const EnvironmentMap& Raw() const { ENGINE_ASSERT(Ready()); return m_map; }

	friend class EnvironmentMapLoader;

private:
	EnvironmentMap m_map;
};

} // engine