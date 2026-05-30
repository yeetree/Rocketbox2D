#ifndef RHI_VULKAN_VULKANRESOURCEDATA
#define RHI_VULKAN_VULKANRESOURCEDATA

#include "Engine/RHI/RHI.h"

#include "RHI/Vulkan/VulkanConstants.h"

#include <vector>
#include <array>
#include <unordered_map>

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Engine::RHI::Vulkan
{
    struct VulkanBufferData {
        BufferDesc desc;
        
        // Static
        vk::Buffer    buffer     = nullptr;
        VmaAllocation allocation = nullptr;

        // Dynamic
        std::array<size_t, k_MaxFramesInFlight> dynamicOffsets = {};
    };

    struct VulkanTextureData {
        TextureDesc         desc;
        vk::Image           image      = nullptr;
        VmaAllocation       allocation = nullptr;
        vk::Format          format;
        vk::raii::ImageView imageView  = nullptr;
        vk::raii::Sampler   sampler    = nullptr;
        bool                ownsImage  = true;
    };

    struct VulkanShaderData {
        std::vector<vk::raii::ShaderModule> modules;
        struct StageInfo {
            uint32_t    moduleIndex;
            std::string entryPoint;
        };
        std::unordered_map<ShaderStage, StageInfo> stages;
    };

    struct VulkanPipelineData {
        vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
        vk::raii::PipelineLayout      pipelineLayout      = nullptr;
        vk::raii::Pipeline            pipeline            = nullptr;
    };

    struct VulkanSwapChainData {
        // Config
        uint32_t      width        = 0;
        uint32_t      height       = 0;
        PresentMode   presentMode  = PresentMode::VSync;
        PixelFormat   format       = PixelFormat::RGBA8;
        bool          needsRebuild = false;

        // Vulkan
        vk::raii::SurfaceKHR   surface   = nullptr;
        vk::raii::SwapchainKHR swapchain = nullptr;
        vk::Extent2D           extent;
        vk::SurfaceFormatKHR   surfaceFormat;
        vk::PresentModeKHR     vkPresentMode;

        // Images
        std::vector<vk::Image>         images;
        std::vector<VulkanTextureData> textures;
        uint32_t                       acquiredImageIndex = 0;

        // Sync
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    };
} // namespace Engine::RHI::Vulkan


#endif // RHI_VULKAN_VULKANRESOURCEDATA
