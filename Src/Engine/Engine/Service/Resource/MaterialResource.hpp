#pragma once

#include <Engine/Service/Resource/Resource.hpp>
#include <Engine/Service/Resource/Loader.hpp>
#include <Engine/Service/Render/Material.hpp>
#include <RHI/Texture.hpp>
#include <RHI/RenderPass.hpp>
#include <RHI/RenderPassDescriptor.hpp>
#include <taskflow/taskflow.hpp>
#include <optional>
#include <string>
#include <cstdint>

namespace engine
{

class MaterialResource;

// Структуры для сериализации материала
struct MaterialDependency 
{
    std::string path;
    int32_t index = 0;
    bool hasDependency = false; // флаг наличия dependency
};

struct MaterialAttachment 
{
    rhi::AttachmentLoadOperation loadOperation = rhi::AttachmentLoadOperation::CLEAR;
    rhi::AttachmentStoreOperation storeOperation = rhi::AttachmentStoreOperation::STORE;
    MaterialDependency dependency;
};

struct MaterialData 
{
    std::string name;
    std::string shader;
    uint8_t version = 0;
    bool offscreen = true;
    std::string depthCompareOp = "LESS";
    std::string cullMode = "BACK";
    bool compute = false; // вместо optional<bool>
    eastl::vector<MaterialAttachment> attachments;
    MaterialAttachment depthAttachment;
    bool hasDepthAttachment = false; // флаг наличия depth attachment
};

class ENGINE_API MaterialLoader final : public Loader
{
public:
	MaterialLoader();

	virtual void					Update() override;

	virtual ResPtr<IResource>		Load(const fs::path& path, bool immediate = false) override;

	virtual ResPtr<IResource>		Get(const fs::path& path) const override;

	virtual void					LoadSystemResources() override;

	const ResPtr<rhi::Pipeline>&	Pipeline(const ResPtr<MaterialResource>& res) const;

	// Called automatically, don't call it unless you know what are you doing!!!
	void							ResizePipelines(glm::ivec2 extent, bool offscreen = true);

	const ResPtr<MaterialResource>& RenderMaterial() const { return m_renderMaterial; }
	const ResPtr<MaterialResource>& PresentMaterial() const { return m_presentMaterial; }

private:
	struct LoadAttachmentDescriptor
	{
		rhi::AttachmentDescriptor m_descriptor;
		fs::path m_dependency;
		int m_depAttachmentIndex;
	};

	struct ParsedPipelineInfo
	{
		eastl::vector<LoadAttachmentDescriptor>		m_attachments;
		std::optional<LoadAttachmentDescriptor>		m_depthAttachment;
		std::shared_ptr<rhi::Shader>				m_shader;
		glm::ivec2									m_viewportSize = { 0, 0 };
		bool										m_compute = false;
		bool										m_offscreen = true; // ignored in compute
		rhi::CompareOp								m_depthCompareOp = rhi::CompareOp::LESS; // ignored in compute
		rhi::CullMode								m_cullMode = rhi::CullMode::BACK; // ignored in compute
	};

	bool							Load(const ResPtr<MaterialResource>& resource, bool forcePipelineRecreation = false);
	MaterialData					ParseMaterialData(const std::string& jsonContent);
	std::shared_ptr<rhi::Pipeline>	AllocatePipeline(ParsedPipelineInfo& info);

	mutable std::mutex																	m_mutex;
	eastl::vector<tf::Future<void>>														m_loadingTasks;
	std::shared_ptr<rhi::ShaderCompiler>												m_shaderCompiler;
	eastl::unordered_map<io::fs::path, std::shared_ptr<rhi::Shader>>					m_shaderCache;
	eastl::unordered_map<std::shared_ptr<rhi::Shader>, std::shared_ptr<rhi::Pipeline>>	m_shaderToPipeline;
	eastl::unordered_map<fs::path, ResPtr<MaterialResource>>							m_cache;
	ResPtr<MaterialResource>															m_renderMaterial;
	ResPtr<MaterialResource>															m_presentMaterial;
};

class ENGINE_API MaterialResource final : public Resource<MaterialResource>
{
public:
	MaterialResource(const io::fs::path& path);
	virtual ~MaterialResource() {}

	const ResPtr<render::Material>& Material() const { ENGINE_ASSERT(Ready()); return m_material; }

	friend class MaterialLoader;

private:
	ResPtr<render::Material> m_material;
};

} // engine