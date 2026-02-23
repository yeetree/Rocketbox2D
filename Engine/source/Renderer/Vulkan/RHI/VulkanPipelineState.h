#ifndef RENDERER_VULKAN_VULKANPIPELINESTATE
#define RENDERER_VULKAN_VULKANPIPELINESTATE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    class VulkanGraphicsDevice;
    class VulkanTexture;

    class VulkanPipelineState : public IPipelineState {
    public:
        VulkanPipelineState(VulkanGraphicsDevice* graphicsDevice, const PipelineDesc& desc);
        ~VulkanPipelineState() override;

        // Getters
        vk::raii::Pipeline& GetPipeline();
        vk::raii::PipelineLayout& GetPipelineLayout();
        ShaderLayout& GetShaderLayout();
        std::map<uint32_t, vk::raii::DescriptorSetLayout>& GetDescriptorSetLayouts();

    private:
        VulkanGraphicsDevice* m_GraphicsDevice;

        // Vulkan members
        vk::raii::Pipeline m_Pipeline = nullptr;
        vk::raii::PipelineLayout m_PipelineLayout = nullptr;
        std::map<uint32_t, vk::raii::DescriptorSetLayout> m_DescriptorSetLayouts;

        ShaderLayout m_ShaderLayout;

        static vk::Format GetVulkanFormat(VertexElementType type);
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
        static vk::PrimitiveTopology GetVulkanTopology(PrimitiveTopology topology);
        static vk::PolygonMode GetVulkanFillMode(FillMode mode);
        static vk::CullModeFlagBits GetVulkanCullMode(CullMode mode);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANPIPELINESTATE
