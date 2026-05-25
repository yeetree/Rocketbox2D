#include "RHI/Vulkan/RHI/VulkanTexture.h"

namespace Engine
{
    // For VulkanGraphicsDevice
    VulkanTexture::VulkanTexture(VulkanContext* context, const TextureDesc& desc)
        : ITexture(desc.width, desc.height, desc.format, desc.usage), m_OwnImage(true), m_Context(context)
    {
        // TODO: Vulkan: Create image
    }

    // For VulkanSwapChain
    VulkanTexture::VulkanTexture(VulkanContext* context, vk::Image image, vk::Format format, const TextureDesc& desc)
        : ITexture(desc.width, desc.height, desc.format, desc.usage), m_OwnImage(false), m_Context(context), m_Image(image), m_ImageFormat(format)
    {
        CreateImageView();
    }

    void VulkanTexture::CreateImageView()
    {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            m_Image,
            vk::ImageViewType::e2D,
            m_ImageFormat,
            {},
            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
        );

        m_ImageView = (*m_Context->GetDevice()).createImageView(imageViewCreateInfo);
    }

    VulkanTexture::~VulkanTexture()
    {
        if(m_ImageView)
        {
            (*m_Context->GetDevice()).destroyImageView(m_ImageView, nullptr);
        }

        if(m_OwnImage && m_Image)
        {
            // TODO: Vulkan: Destroy image
        }
    }
} // namespace Engine
