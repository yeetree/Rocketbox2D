#ifndef RENDERER_VULKAN_VULKANPIPELINESTATE
#define RENDERER_VULKAN_VULKANPIPELINESTATE

#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    class VulkanGraphicsDevice;
    class VulkanTexture;
    class VulkanUniformBuffer;

    class VulkanPipelineState : public IPipelineState {
    public:
        VulkanPipelineState(VulkanGraphicsDevice* graphicsDevice, const PipelineDesc& desc);
        ~VulkanPipelineState() override;

        // Getters
        vk::raii::Pipeline& GetPipeline();
        vk::raii::PipelineLayout& GetLayout();

        vk::DescriptorSet GetDescriptorSetForTexture(VulkanTexture& tex, uint32_t slot);
        vk::DescriptorSet GetDescriptorSetForUniformBuffer(VulkanUniformBuffer& ubo, uint32_t slot);

    private:
        VulkanGraphicsDevice* m_GraphicsDevice;

        // Vulkan members
        vk::raii::Pipeline m_Pipeline = nullptr;
        vk::raii::PipelineLayout m_Layout = nullptr;
        std::map<uint32_t, vk::raii::DescriptorSetLayout> m_DescriptorSetLayouts;

        // id + slot for resources
        struct ResourceKey {
            uint32_t id, slot, frameIndex;

            bool operator<(const ResourceKey& other) const {
                return std::tie(id, slot, frameIndex) < std::tie(other.id, other.slot, other.frameIndex);
            }
        };

        // Maps pair (Texture ID and Slot ID) to a descriptor cache
        std::map<ResourceKey, vk::raii::DescriptorSet> m_DescriptorCacheUniformBufferObjects, m_DescriptorCacheTextures;

        // Maps slot to descriptor set
        struct SetBinding {
            uint32_t set, binding;
            bool operator<(const SetBinding& other) const {
                return std::tie(set, binding) < std::tie(other.set, other.binding);
            }
        };
        std::map<uint32_t, SetBinding> m_SlotToSetBindingMap;

        // Descriptor set layouts
        vk::DescriptorSetLayout m_TextureDescriptorSetLayout, m_BufferDescriptorSetLayout;

        static vk::Format GetVulkanFormat(VertexElementType type);
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
        static vk::PrimitiveTopology GetVulkanTopology(PrimitiveTopology topology);
        static vk::PolygonMode GetVulkanFillMode(FillMode mode);
        static vk::CullModeFlagBits GetVulkanCullMode(CullMode mode);
    };
} // namespace Engine

#endif // RENDERER_VULKAN_VULKANPIPELINESTATE
