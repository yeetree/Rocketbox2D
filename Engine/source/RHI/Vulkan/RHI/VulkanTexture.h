#ifndef RHI_VULKAN_RHI_VULKANTEXTURE
#define RHI_VULKAN_RHI_VULKANTEXTURE

#include "Engine/RHI/ITexture.h"

#include "RHI/Vulkan/VulkanContext.h"

#include <vulkan/vulkan_raii.hpp>

namespace Engine
{
    class ENGINE_EXPORT VulkanTexture : public ITexture
    {
    private:
        // Vulkan
        VulkanContext* m_Context;
        vk::Image m_Image = nullptr; // Either created in constructor or swapchain
        VmaAllocation m_Allocation;
        vk::Format m_ImageFormat; // Retrieved from desc.format or swapchain
        vk::raii::ImageView m_ImageView = nullptr; // Created in CreateImageView 
        vk::raii::Sampler m_Sampler = nullptr; // Created in CreateSampler if sampled

        // State
        bool m_OwnImage; // Whether or not we created m_Image and can destroy it

        // Private
        void CreateImageView();
        void CreateSampler();

    public:
        // For VulkanGraphicsDevice
        VulkanTexture(VulkanContext* context, const TextureDesc& desc);

        // For VulkanSwapChain
        VulkanTexture(VulkanContext* context, vk::Image image, vk::Format format, const TextureDesc& desc);

        ~VulkanTexture();

        vk::Image& GetImage() { return m_Image; }
        vk::raii::ImageView& GetImageView() { return m_ImageView; }
        vk::raii::Sampler& GetSampler() { return m_Sampler; }
    };
} // namespace Engine


#endif // RHI_VULKAN_RHI_VULKANTEXTURE
