#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanConstants.h"

namespace Engine::RHI::Vulkan::VulkanCommon
{
    void TransitionImageLayout(
        vk::CommandBuffer        cmd,
        vk::Image                image,
        vk::ImageLayout          oldLayout,
        vk::ImageLayout          newLayout,
        vk::AccessFlags2         srcAccess,
        vk::AccessFlags2         dstAccess,
        vk::PipelineStageFlags2  srcStage,
        vk::PipelineStageFlags2  dstStage)
    {
        vk::ImageMemoryBarrier2 barrier;
        barrier.srcStageMask        = srcStage;
        barrier.srcAccessMask       = srcAccess;
        barrier.dstStageMask        = dstStage;
        barrier.dstAccessMask       = dstAccess;
        barrier.oldLayout           = oldLayout;
        barrier.newLayout           = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = image;
        barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        vk::DependencyInfo dependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers    = &barrier;
        
        cmd.pipelineBarrier2(dependencyInfo);
    }

    vk::SurfaceFormatKHR GetSurfaceFormat(PixelFormat format)
    {
        vk::SurfaceFormatKHR surfaceFormat(vk::Format::eUndefined);

        switch(format)
        {
            case PixelFormat::RGBA8:
            {
                surfaceFormat.format = vk::Format::eR8G8B8A8Srgb;
                surfaceFormat.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
                break;
            }
        }

        return surfaceFormat;
    }

    vk::SurfaceFormatKHR ChooseSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats, vk::SurfaceFormatKHR requestedFormat)
    {   
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat == requestedFormat) {
                return requestedFormat;
            }
        }

        return vk::SurfaceFormatKHR(vk::Format::eUndefined);
    }

    vk::PresentModeKHR GetPresentMode(PresentMode presentMode)
    {
        switch(presentMode)
        {
            case PresentMode::Immediate: return vk::PresentModeKHR::eImmediate; break;
            case PresentMode::VSync:     return vk::PresentModeKHR::eFifo; break;
            case PresentMode::Mailbox:   return vk::PresentModeKHR::eMailbox; break;
        }

        return vk::PresentModeKHR::eImmediate; // Shouldn't really happen...
    }

    vk::PresentModeKHR ChoosePresentMode(const std::vector<vk::PresentModeKHR>& availableModes, vk::PresentModeKHR requestedMode)
    {
        for (const auto& availableMode : availableModes) {
            if (availableMode == requestedMode) {
                return requestedMode;
            }
        }

        return vk::PresentModeKHR::eImmediate;
    }

    vk::Extent2D GetSurfaceExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    uint32_t GetSurfaceMinImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
    {
        uint32_t minImageCount = std::max((uint32_t)k_MaxFramesInFlight, capabilities.minImageCount);
        if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount))
        {
            minImageCount = capabilities.maxImageCount;
        }
        return minImageCount;
    }

    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(vk::raii::Device& device, const std::vector<uint32_t>& code) {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();
        return vk::raii::ShaderModule{ device, createInfo };
    }

    vk::ShaderStageFlagBits GetShaderStage(ShaderStage stage)
    {
        vk::ShaderStageFlagBits ss;
        switch(stage)
        {
            case ShaderStage::Vertex: ss = vk::ShaderStageFlagBits::eVertex; break;
            case ShaderStage::Fragment: ss = vk::ShaderStageFlagBits::eFragment; break;
        }
        return ss;
    }

    vk::PrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology)
    {
        vk::PrimitiveTopology t;
        switch(topology)
        {
            case PrimitiveTopology::LineList: t = vk::PrimitiveTopology::eLineList; break;
            case PrimitiveTopology::PointList: t = vk::PrimitiveTopology::ePointList; break;
            case PrimitiveTopology::TriangleList: t = vk::PrimitiveTopology::eTriangleList; break;
        }
        return t;
    }

    vk::PolygonMode GetPolygonMode(PolygonMode mode)
    {
        vk::PolygonMode m;
        switch(mode)
        {
            case PolygonMode::Fill: m = vk::PolygonMode::eFill; break;
            case PolygonMode::Line: m = vk::PolygonMode::eLine; break;
            case PolygonMode::Point: m = vk::PolygonMode::ePoint; break;
        }
        return m;
    }

    vk::CullModeFlags GetCullMode(CullMode mode)
    {
        vk::CullModeFlags f;
        switch(mode)
        {
            case CullMode::None: f = vk::CullModeFlagBits::eNone; break;
            case CullMode::Back: f = vk::CullModeFlagBits::eBack; break;
            case CullMode::Front: f = vk::CullModeFlagBits::eFront; break;
        }
        return f;
    }

    vk::FrontFace GetFrontFace(FrontFace frontFace)
    {
        vk::FrontFace f;
        switch(frontFace)
        {
            case FrontFace::Clockwise: f = vk::FrontFace::eClockwise; break;
            case FrontFace::CounterClockwise: f = vk::FrontFace::eCounterClockwise; break;
        }
        return f;
    }

    vk::Format GetVertexElementFormat(VertexElementType type)
    {
        vk::Format e;
        switch(type)
        {
            case VertexElementType::Int: e = vk::Format::eR32Sint; break;
            case VertexElementType::Float: e = vk::Format::eR32Sfloat; break;
            case VertexElementType::Vec2: e = vk::Format::eR32G32Sfloat; break;
            case VertexElementType::Vec3: e = vk::Format::eR32G32B32Sfloat; break;
            case VertexElementType::Vec4: e = vk::Format::eR32G32B32A32Sfloat; break;
        }
        return e;
    }

} // namespace Engine::RHI::Vulkan::VulkanCommon