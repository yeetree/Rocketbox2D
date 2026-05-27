#ifndef RHI_VULKAN_RHI_VULKANPIPELINE
#define RHI_VULKAN_RHI_VULKANPIPELINE

#include "Engine/RHI/IPipeline.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanPipeline : public IPipeline
    {
    private:

        // Vulkan
        vk::raii::DescriptorSetLayout m_DescriptorSetLayout = nullptr;
        vk::raii::PipelineLayout m_Layout = nullptr;
        vk::raii::Pipeline m_Pipeline = nullptr;

        // Helpers
        static vk::ShaderStageFlagBits GetShaderStage(ShaderStage stage);
        static vk::PrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology);
        static vk::Format GetVertexElementFormat(VertexElementType type);

    public:
        VulkanPipeline(VulkanContext* context, const PipelineDesc& desc);
        ~VulkanPipeline() override = default;

        // Vulkan
        vk::raii::DescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
        vk::raii::PipelineLayout& GetPipelineLayout() { return m_Layout; }
        vk::raii::Pipeline& GetPipeline() { return m_Pipeline; }
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANPIPELINE
