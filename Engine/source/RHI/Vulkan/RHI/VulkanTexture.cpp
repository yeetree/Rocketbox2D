#include "RHI/Vulkan/RHI/VulkanTexture.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    // For VulkanGraphicsDevice
    VulkanTexture::VulkanTexture(VulkanContext* context, const TextureDesc& desc)
        : ITexture(desc.width, desc.height, desc.format, desc.usage), m_OwnImage(true), m_Context(context)
    {
        ENGINE_CORE_ASSERT(context != nullptr, "Vulkan: VulkanTexture(): context is nullptr!");

         // Create image
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = GetWidth();
        imageInfo.extent.height = GetHeight();
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // TODO: Vulkan: Select proper texture format
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        
        VkImage image = nullptr;

        vmaCreateImage(
            m_Context->GetAllocator(), 
            &imageInfo, 
            &allocInfo, 
            &image, 
            &m_Allocation, 
            nullptr
        );

        m_Image = image;

        if(GetUsage() & TextureUsage::Sampled > 0)
        {
            CreateSampler();
        }
    }

    // For VulkanSwapChain
    VulkanTexture::VulkanTexture(VulkanContext* context, vk::Image image, vk::Format format, const TextureDesc& desc)
        : ITexture(desc.width, desc.height, desc.format, desc.usage), m_OwnImage(false), m_Context(context), m_Image(image), m_ImageFormat(format)
    {
        CreateImageView();

        if(GetUsage() & TextureUsage::Sampled > 0)
        {
            CreateSampler();
        }
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

        m_ImageView = vk::raii::ImageView(m_Context->GetDevice(), imageViewCreateInfo);
    }

    void VulkanTexture::CreateSampler()
    {
        // Create sampler
        // TODO: Vulkan: Texture sampling options
        // Hardcoded: linear sampling, repeat
        vk::SamplerCreateInfo samplerInfo{};
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear;
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = vk::False;
        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = vk::False;
        samplerInfo.compareEnable = vk::False;
        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

        m_Sampler = vk::raii::Sampler(m_Context->GetDevice(), samplerInfo);
    }

    VulkanTexture::~VulkanTexture()
    {
        if(m_OwnImage && m_Image)
        {
            vmaDestroyImage(m_Context->GetAllocator(), m_Image, m_Allocation);
        }
    }
} // namespace Engine
