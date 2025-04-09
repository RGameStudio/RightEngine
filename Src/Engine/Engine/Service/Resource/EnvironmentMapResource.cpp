#include <Engine/Service/Resource/EnvironmentMapResource.hpp>
#include <Engine/Service/Render/RenderService.hpp>
#include <RHI/Pipeline.hpp>

RTTR_REGISTRATION
{
	using namespace engine::registration;

	ResourceLoader<engine::EnvironmentMapLoader>("engine::EnvironmentMapLoader")
		.Domain(engine::Domain::UI);
}

namespace engine
{

EnvironmentMapLoader::EnvironmentMapLoader()
{
}

void EnvironmentMapLoader::Update()
{
}

ResPtr<IResource> EnvironmentMapLoader::Load(const fs::path& path, bool immediate)
{
	std::lock_guard l(m_mutex);

	if (auto res = Get(path))
	{
		return res;
	}

	auto resource = MakeResPtr<EnvironmentMapResource>(path);
	resource->m_status = IResource::Status::LOADING;
	m_cache[path] = resource;

	auto load = [this, resource]
	{
		PROFILER_CPU_ZONE_NAME("Load env map");
		const auto result = Load(resource);
		resource->m_status = result ? IResource::Status::READY : IResource::Status::FAILED;
	};

	if (immediate)
	{
		load();
		return resource;
	}

	auto& ts = Instance().Service<ThreadService>();
	ts.AddBackgroundTask([load]()
		{
			load();
		});

	return resource;
}

ResPtr<IResource> EnvironmentMapLoader::Get(const fs::path& path) const
{
	if (const auto it = m_cache.find(path); it != m_cache.end())
	{
		return it->second;
	}
	return {};
}

void EnvironmentMapLoader::LoadSystemResources()
{
	auto& rs = Instance().Service<ResourceService>();

	m_equirectToCubemapMaterial = std::static_pointer_cast<MaterialResource>(rs.Load<MaterialResource>("/System/Materials/equirect_to_cubemap.material"));
	m_envmapIrradianceMaterial = std::static_pointer_cast<MaterialResource>(rs.Load<MaterialResource>("/System/Materials/envmap_irradiance.material"));
	m_envmapPrefilterMaterial = std::static_pointer_cast<MaterialResource>(rs.Load<MaterialResource>("/System/Materials/envmap_prefilter.material"));
	m_skyboxMaterial = std::static_pointer_cast<MaterialResource>(rs.Load<MaterialResource>("/System/Materials/skybox.material"));

	m_equirectToCubemapMaterial->Wait();
	m_envmapIrradianceMaterial->Wait();
	m_envmapPrefilterMaterial->Wait();
	m_skyboxMaterial->Wait();
}

bool EnvironmentMapLoader::Load(const ResPtr<EnvironmentMapResource>& resource)
{
	auto& rs = Instance().Service<RenderService>();
	auto& resourceService = Instance().Service<ResourceService>();
	const auto envTex = resourceService.Load<TextureResource>(resource->SourcePath(), true);
	envTex->Wait();

	EnvironmentMap data{};

	// Equirect to cubemap
	{
		rhi::TextureDescriptor envCubemapDesc{};
		envCubemapDesc.m_type = rhi::TextureType::TEXTURE_CUBEMAP;
		envCubemapDesc.m_format = rhi::Format::RGBA16_SFLOAT;
		envCubemapDesc.m_layersAmount = 6;
		envCubemapDesc.m_width = 1024;
		envCubemapDesc.m_height = 1024;

		const auto envCubemap = rs.CreateTexture(envCubemapDesc);

		auto& computePass = rs.Pipeline(m_equirectToCubemapMaterial)->Descriptor().m_computePass;
		computePass->m_textures.emplace_back(envTex->Texture());
		computePass->m_storageTextures.emplace_back(envCubemap);

		m_equirectToCubemapMaterial->Material()->SetTexture(envCubemap, 0);
		m_equirectToCubemapMaterial->Material()->SetTexture(envTex->Texture(), 1);
		m_equirectToCubemapMaterial->Material()->Sync();

		const auto state = rs.BeginComputePassImmediate(m_equirectToCubemapMaterial);
		rs.BindMaterial(m_equirectToCubemapMaterial, state);
		rs.Dispatch(envCubemap->Width() / 32, envCubemap->Height() / 32, 6, state);
		rs.EndComputePass(m_equirectToCubemapMaterial, state);

		data.m_cubemap = envCubemap;
	}

	// Compute irradiance
	{
		rhi::TextureDescriptor irrCubemapDesc{};
		irrCubemapDesc.m_type = rhi::TextureType::TEXTURE_CUBEMAP;
		irrCubemapDesc.m_format = rhi::Format::RGBA16_SFLOAT;
		irrCubemapDesc.m_layersAmount = 6;
		irrCubemapDesc.m_width = 32;
		irrCubemapDesc.m_height = 32;

		const auto irrCubemap = rs.CreateTexture(irrCubemapDesc);

		auto& computePass = rs.Pipeline(m_envmapIrradianceMaterial)->Descriptor().m_computePass;
		computePass->m_textures.emplace_back(data.m_cubemap);
		computePass->m_storageTextures.emplace_back(irrCubemap);

		m_envmapIrradianceMaterial->Material()->SetTexture(irrCubemap, 0);
		m_envmapIrradianceMaterial->Material()->SetTexture(data.m_cubemap, 1);
		m_envmapIrradianceMaterial->Material()->Sync();

		const auto state = rs.BeginComputePassImmediate(m_envmapIrradianceMaterial);
		rs.BindMaterial(m_envmapIrradianceMaterial, state);
		rs.Dispatch(irrCubemap->Width() / 32, irrCubemap->Height() / 32, 6, state);
		rs.EndComputePass(m_envmapIrradianceMaterial, state);

		data.m_irradianceTexture = irrCubemap;
	}

	// Compute prefilter map
	{
		rhi::TextureDescriptor prefilterDesc{};
		prefilterDesc.m_type = rhi::TextureType::TEXTURE_CUBEMAP;
		prefilterDesc.m_format = rhi::Format::RGBA16_SFLOAT;
		prefilterDesc.m_layersAmount = 6;
		prefilterDesc.m_mipmapped = true;
		prefilterDesc.m_width = 32;
		prefilterDesc.m_height = 32;

		const auto prefilterCubemap = rs.CreateTexture(prefilterDesc);

		auto& computePass = rs.Pipeline(m_envmapPrefilterMaterial)->Descriptor().m_computePass;
		computePass->m_textures.emplace_back(data.m_cubemap);
		computePass->m_storageTextures.emplace_back(prefilterCubemap);

		const auto maxMipLevel = prefilterCubemap->CalculateMipCount();

		for (uint8_t mipLevel = 0; mipLevel < maxMipLevel; mipLevel++)
		{
			m_envmapPrefilterMaterial->Material()->SetTexture(prefilterCubemap, 0, mipLevel);
			m_envmapPrefilterMaterial->Material()->SetTexture(data.m_cubemap, 1);
			m_envmapPrefilterMaterial->Material()->Sync();

			const float roughness = static_cast<float>(mipLevel) / static_cast<float>(maxMipLevel);

			const auto state = rs.BeginComputePassImmediate(m_envmapPrefilterMaterial);
			rs.BindMaterial(m_envmapPrefilterMaterial, state);
			rs.PushConstantComputeImmediate(&roughness, sizeof(roughness), m_envmapPrefilterMaterial, state);
			rs.Dispatch(prefilterCubemap->Width() / 32, prefilterCubemap->Height() / 32, 6, state);
			rs.EndComputePass(m_envmapPrefilterMaterial, state);
		}

		data.m_prefilterTexture = prefilterCubemap;
	}

	resource->m_map = data;
	return true;
}

EnvironmentMapResource::EnvironmentMapResource(const io::fs::path& path) : Resource(path)
{
}

} // engine
