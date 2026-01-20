#ifndef RENDERER_VULKAN_VULKANPIPELINESTATE
#define RENDERER_VULKAN_VULKANPIPELINESTATE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    class VulkanGraphicsDevice;

    struct VulkanPipelineState : public IPipelineState {
    public:
        VulkanPipelineState(VulkanGraphicsDevice* graphicsDevice, const PipelineDesc& desc);
        ~VulkanPipelineState() override;

        vk::raii::Pipeline m_Pipeline = nullptr;

    private:
        static vk::Format GetVulkanFormat(VertexElementType type);
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANPIPELINESTATE
