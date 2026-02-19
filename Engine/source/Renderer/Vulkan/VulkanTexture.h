#ifndef RENDERER_VULKAN_VULKANTEXTURE
#define RENDERER_VULKAN_VULKANTEXTURE

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

#include "Engine/Renderer/RHI/ITexture.h"

namespace Engine
{
    // fwd
    class VulkanGraphicsDevice;

    class VulkanTexture : public ITexture {
    public:
        VulkanTexture(VulkanGraphicsDevice* graphicsDevice, const TextureDesc& desc);
        ~VulkanTexture() override;

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
        TextureFormat GetFormat() const override;

        // Getter for VulkanGraphicsDevice
        vk::DescriptorSet GetDescriptorSet() const;

    private:
        uint32_t m_Width, m_Height;
        TextureFormat m_Format;

        VulkanGraphicsDevice* m_GraphicsDevice;

        // VMA members
        VkImage m_Image;
        VmaAllocation m_Allocation;

        // Vulkan members
        vk::raii::ImageView m_ImageView = nullptr;
        vk::raii::Sampler m_Sampler = nullptr;
        vk::raii::DescriptorSet m_DescriptorSet = nullptr;
    };
} // namespace Engine


#endif // RENDERER_VULKAN_VULKANTEXTURE
