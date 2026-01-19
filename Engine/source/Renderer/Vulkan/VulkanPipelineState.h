#ifndef RENDERER_VULKAN_VULKANPIPELINESTATE
#define RENDERER_VULKAN_VULKANPIPELINESTATE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    struct VulkanPipelineState : public IPipelineState {
    public:
        VulkanPipelineState(const vk::raii::Device& device, const vk::Format& colorAttachmentFormat, const PipelineDesc& desc);
        ~VulkanPipelineState() override;

        vk::raii::Pipeline m_Pipeline = nullptr;

    private:
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANPIPELINESTATE
