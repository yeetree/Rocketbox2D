#ifndef RENDERER_VULKAN_VULKANGRAPHICSDEVICE
#define RENDERER_VULKAN_VULKANGRAPHICSDEVICE

#include <SDL3/SDL.h>
#include <vulkan/vulkan_raii.hpp>

#include "Engine/Renderer/RHI/IGraphicsDevice.h"

constexpr std::array<char const*, 1> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

constexpr std::array<const char*, 4> deviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

namespace Engine {
    // Vulkan Implementation of IGraphicsDevice
    class VulkanGraphicsDevice : public IGraphicsDevice {
    public:
        // Constructor: Creates an VulkanGraphicsDevice with SDL_Window
        VulkanGraphicsDevice(SDL_Window* window);
        virtual ~VulkanGraphicsDevice();

        // Prevent copying
        VulkanGraphicsDevice(const VulkanGraphicsDevice&) = delete;
        VulkanGraphicsDevice& operator=(const VulkanGraphicsDevice&) = delete;

        // Resource Creation
        Scope<IBuffer> CreateBuffer(const BufferDesc& desc) override;
        Scope<ITexture> CreateTexture(const TextureDesc& desc) override;
        Scope<IShader> CreateShader(const ShaderDesc& desc) override;
        Scope<IPipelineState> CreatePipelineState(const PipelineDesc& desc) override;
        Scope<IVertexArray> CreateVertexArray(const VertexArrayDesc& desc) override;

        // Frame Management
        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void SetClearColor(Vec4 color) override;

        // Draw call
        void SubmitDraw(uint32_t indexCount) override;

        // Utility
        //ITexture* GetBackBuffer() override { return m_BackBuffer.get(); }

        // Resize
        void Resize(int width, int height) override;

    private:
        // Debug Callback
        static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

        // Setup functions
        void CreateInstance();
        void SetupDebugMessanger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateGraphicsPipeline();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();

        // Utility
        [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code) const;
        static vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        static vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
        static uint32_t ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);
        void TransitionImageLayout(
            uint32_t                imageIndex,
            vk::ImageLayout         old_layout,
            vk::ImageLayout         new_layout,
            vk::AccessFlags2        src_access_mask,
            vk::AccessFlags2        dst_access_mask,
            vk::PipelineStageFlags2 src_stage_mask,
            vk::PipelineStageFlags2 dst_stage_mask);

        SDL_Window* m_Window;

        vk::raii::Context  m_Context;
        vk::raii::Instance m_Instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
        vk::raii::SurfaceKHR m_Surface = nullptr;
        vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
        vk::raii::Device m_Device = nullptr;
        uint32_t m_QueueIndex = ~0;
	    vk::raii::Queue m_Queue = nullptr;
        vk::raii::SwapchainKHR m_SwapChain = nullptr;
        std::vector<vk::Image> m_SwapChainImages;
        vk::SurfaceFormatKHR m_SwapChainSurfaceFormat;
        vk::Extent2D m_SwapChainExtent;
        std::vector<vk::raii::ImageView> m_SwapChainImageViews;
        vk::raii::PipelineLayout m_PipelineLayout = nullptr;
        vk::raii::Pipeline m_GraphicsPipeline = nullptr;
        vk::raii::CommandPool m_CommandPool = nullptr;  
        vk::raii::CommandBuffer m_CommandBuffer = nullptr;
        
        uint32_t m_CurrentFrame = 0;
        uint32_t m_ImageIndex = 0;
    
        // Sync objects
        vk::raii::Semaphore m_PresentCompleteSemaphore = nullptr;
        vk::raii::Semaphore m_RenderFinishedSemaphore  = nullptr;
        vk::raii::Fence     m_DrawFence                = nullptr;

        // Scope<ITexture> m_BackBuffer;
    };

} // namespace Engine

#endif // RENDERER_VULKAN_VULKANGRAPHICSDEVICE
