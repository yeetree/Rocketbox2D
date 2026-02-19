#include "Renderer/Vulkan/RHI/VulkanTexture.h"
#include "Renderer/Vulkan/RHI/VulkanGraphicsDevice.h"
#include "Renderer/Vulkan/RHI/VulkanBuffer.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanTexture::VulkanTexture(VulkanGraphicsDevice* graphicsDevice, const TextureDesc& desc) 
        : m_GraphicsDevice(graphicsDevice), m_Width(desc.width), m_Height(desc.height), m_Format(desc.format)
    {
        ENGINE_CORE_ASSERT(graphicsDevice != nullptr, "Vulkan: invalid graphics device when creating texture!");

        // Create image
        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = m_Width;
        imageInfo.extent.height = m_Height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // hardcoded for now
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        
        vmaCreateImage(
            m_GraphicsDevice->GetDevice().GetAllocator(), 
            &imageInfo, 
            &allocInfo, 
            &m_Image, 
            &m_Allocation, 
            nullptr
        );

        // Create image view 
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.image = m_Image;
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = vk::Format::eR8G8B8A8Srgb; 
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        m_ImageView = vk::raii::ImageView(m_GraphicsDevice->GetDevice().GetDevice(), viewInfo);

        // Create sampler
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

        m_Sampler = vk::raii::Sampler(m_GraphicsDevice->GetDevice().GetDevice(), samplerInfo);

        // Upload data

        if(desc.data != nullptr) {
            // Create the staging buffer
            BufferDesc stagingDesc;
            stagingDesc.size = m_Width * m_Height * 4; // RGBA8
            stagingDesc.type = BufferType::Vertex;  // Generic memory type
            stagingDesc.isDynamic = true; // map memory
            stagingDesc.data = desc.data; // upload pixel data 
            VulkanBuffer stagingBuffer(m_GraphicsDevice, stagingDesc);

            // Copy
            vk::raii::CommandBuffer cmd = m_GraphicsDevice->BeginOneTimeCommands();

            // undefined -> transferdstoptimal
            m_GraphicsDevice->TransitionImageLayout(
                cmd,
                m_Image, 
                vk::ImageLayout::eUndefined, 
                vk::ImageLayout::eTransferDstOptimal,
                vk::AccessFlagBits2::eNone,                   // src access
                vk::AccessFlagBits2::eTransferWrite,          // dst access
                vk::PipelineStageFlagBits2::eTopOfPipe,       // src stage
                vk::PipelineStageFlagBits2::eTransfer         // dst stage
            );

            // copy buffer to image
            vk::BufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = vk::Offset3D{0, 0, 0};
            region.imageExtent = vk::Extent3D{ m_Width, m_Height, 1 };

            cmd.copyBufferToImage(
                static_cast<vk::Buffer>(stagingBuffer.GetBuffer()),
                m_Image,
                vk::ImageLayout::eTransferDstOptimal,
                region
            );

            // transferdstoptimal -> shaderread
            m_GraphicsDevice->TransitionImageLayout(
                cmd,
                m_Image, 
                vk::ImageLayout::eTransferDstOptimal, 
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::AccessFlagBits2::eTransferWrite,          // src access
                vk::AccessFlagBits2::eShaderRead,             // dst access
                vk::PipelineStageFlagBits2::eTransfer,        // src stage
                vk::PipelineStageFlagBits2::eFragmentShader   // dst stage
            );

            m_GraphicsDevice->EndOneTimeCommands(cmd);
        }

        // Create and descriptor set
        vk::DescriptorSetLayout layout = m_GraphicsDevice->GetTextureDescriptorSetLayout();
        
        vk::DescriptorSetAllocateInfo dsAllocInfo; 
        dsAllocInfo.descriptorPool = *m_GraphicsDevice->GetDescriptorPool();
        dsAllocInfo.descriptorSetCount = 1;
        dsAllocInfo.pSetLayouts = &layout;

        auto sets = m_GraphicsDevice->GetDevice().GetDevice().allocateDescriptorSets(dsAllocInfo);
        m_DescriptorSet = std::move(sets.front());

        // Update set for image view and sampler
        vk::DescriptorImageInfo imageInfoDescriptor{};
        imageInfoDescriptor.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfoDescriptor.imageView = *m_ImageView;
        imageInfoDescriptor.sampler = *m_Sampler;

        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = *m_DescriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfoDescriptor;

        m_GraphicsDevice->GetDevice().GetDevice().updateDescriptorSets(descriptorWrite, nullptr);
    }

    VulkanTexture::~VulkanTexture() {
        if (m_Image) {
            vmaDestroyImage(m_GraphicsDevice->GetDevice().GetAllocator(), m_Image, m_Allocation);
        }
    }

    uint32_t VulkanTexture::GetWidth() const {
        return m_Width;
    }

    uint32_t VulkanTexture::GetHeight() const {
        return m_Height;
    }

    TextureFormat VulkanTexture::GetFormat() const {
        return m_Format;
    }

    vk::DescriptorSet VulkanTexture::GetDescriptorSet() const {
        return *m_DescriptorSet;
    }
} // namespace Engine
