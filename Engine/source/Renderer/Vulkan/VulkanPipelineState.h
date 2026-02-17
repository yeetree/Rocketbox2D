#ifndef RENDERER_VULKAN_VULKANPIPELINESTATE
#define RENDERER_VULKAN_VULKANPIPELINESTATE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    class VulkanGraphicsDevice;

    class VulkanPipelineState : public IPipelineState {
    public:
        VulkanPipelineState(VulkanGraphicsDevice* graphicsDevice, const PipelineDesc& desc);
        ~VulkanPipelineState() override;

        // Getters
        vk::raii::Pipeline& GetPipeline();
        vk::raii::PipelineLayout& GetLayout();

    private:
        // Vulkan members
        vk::raii::Pipeline m_Pipeline = nullptr;
        vk::raii::PipelineLayout m_Layout = nullptr;

        static vk::Format GetVulkanFormat(VertexElementType type);
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
        static vk::PrimitiveTopology GetVulkanTopology(PrimitiveTopology topology);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANPIPELINESTATE
