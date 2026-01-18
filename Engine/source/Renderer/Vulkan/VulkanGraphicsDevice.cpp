#include "Renderer/Vulkan/VulkanGraphicsDevice.h"
#include "Engine/Core/Log.h"
#include <SDL3/SDL_vulkan.h>

namespace Engine {
    
    VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanGraphicsDevice::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
        switch(severity) {
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
                LOG_CORE_WARN("Vulkan: Warning: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
                LOG_CORE_ERROR("Vulkan: Error: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
            case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
                LOG_CORE_TRACE("Vulkan: Verbose: Validation Layer {0}: {1}", to_string(type), pCallbackData->pMessage);
                break;
        }
        
        return vk::False;
    }

    VulkanGraphicsDevice::VulkanGraphicsDevice(SDL_Window* window) : m_Window(window) {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");
        
        CreateInstance();
        SetupDebugMessanger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice() {
        // We used VulkanHpp because we are awesome
    };

    // Setup functions
    void VulkanGraphicsDevice::CreateInstance() {
        LOG_CORE_INFO("Vulkan: Creating Vulkan instance...");

        // App info
        constexpr vk::ApplicationInfo appInfo(
            "VKTest",
            VK_MAKE_VERSION( 1, 0, 0 ),
            "Engine",
            VK_MAKE_VERSION( 1, 0, 0 ),
            vk::ApiVersion14
        );

        // Get the required layers
        std::vector<char const*> requiredLayers;
        if (enableValidationLayers) {
            requiredLayers.assign(validationLayers.begin(), validationLayers.end());
        }

        // Check if the required layers are supported by the Vulkan implementation.
        auto layerProperties = m_Context.enumerateInstanceLayerProperties();
        if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
            return std::ranges::none_of(layerProperties,
                                    [requiredLayer](auto const& layerProperty)
                                    { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
        }))
        {
            LOG_CORE_ERROR("Vulkan: One or more required layers is not supported!");
            return; // Failure
        }

        // Get required extensions for SDL
        uint32_t SDLExtensionCount = 0;
        char const * const * SDLExtensions = SDL_Vulkan_GetInstanceExtensions(&SDLExtensionCount);
        std::vector extensions(SDLExtensions, SDLExtensions + SDLExtensionCount);
        
        // Add Engine extensions
        if (enableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName );
        }

        std::vector<vk::ExtensionProperties> extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (uint32_t i = 0; i < extensions.size(); ++i)
        {
            if (std::ranges::none_of(extensionProperties,
                                    [extension = extensions[i]](auto const& extensionProperty)
                                    { return strcmp(extensionProperty.extensionName, extension) == 0; }))
            {
                LOG_CORE_ERROR("Vulkan: Required extension not supported: {0}", std::string(extensions[i]));
                return; // Failure
            }
        }

        // Create instance
        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());;
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
        createInfo.ppEnabledLayerNames = requiredLayers.data(); 

        m_Instance = vk::raii::Instance(m_Context, createInfo);
    }

    void VulkanGraphicsDevice::SetupDebugMessanger() {
        // Set up debug messanger
        if(!enableValidationLayers) { return; }

        LOG_CORE_INFO("Vulkan: Setting up Debug Messanger...");

        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );
        vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );
        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
        debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
        debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
        debugUtilsMessengerCreateInfoEXT.pfnUserCallback = &DebugCallback;
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    void VulkanGraphicsDevice::CreateSurface() {
        LOG_CORE_INFO("Vulkan: Creating surface...");

        // Thank god we use SDL
        VkSurfaceKHR surface;
        if (!SDL_Vulkan_CreateSurface(m_Window, *m_Instance, NULL, &surface)) {
            LOG_CORE_ERROR("Vulkan: Could not create surface: SDL: {0}", SDL_GetError());
            return; // Failure
        }
        m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
    }

    void VulkanGraphicsDevice::PickPhysicalDevice() {
        LOG_CORE_INFO("Vulkan: Selecting Physical Device...");
        
        // Select physical device
        std::vector<vk::raii::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            LOG_CORE_ERROR("Vulkan: Failed to find GPUs with Vulkan support!");
            return; // Failure
        }

        // Loop through devices
        const auto devIter = std::ranges::find_if(devices,
        [&](auto const & device) {
                auto queueFamilies = device.getQueueFamilyProperties();
                bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
                const auto qfpIter = std::ranges::find_if(queueFamilies,
                []( vk::QueueFamilyProperties const & qfp )
                {
                    return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
                } );
                isSuitable = isSuitable && ( qfpIter != queueFamilies.end() );
                auto extensions = device.enumerateDeviceExtensionProperties( );
                bool found = true;
                for (auto const & extension : deviceExtensions) {
                    auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                    found = found &&  extensionIter != extensions.end();
                }
                isSuitable = isSuitable && found;
                if (isSuitable) {
                    m_PhysicalDevice = device;
                }
                return isSuitable;
        });
        if (devIter == devices.end()) {
            LOG_CORE_ERROR("Vulkan: Failed to find suitable GPU!");
            return; // Failure
        }
    }

    void VulkanGraphicsDevice::CreateLogicalDevice() {
        LOG_CORE_INFO("Vulkan: Creating Logical Device...");

        // Get queue family
        
		// find the index of the first queue family that supports graphics
		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();
		
        // get the first index into queueFamilyProperties which supports graphics
		// get the first index into queueFamilyProperties which supports graphics
        std::vector<vk::QueueFamilyProperties>::iterator graphicsQueueFamilyProperty = std::ranges::find_if( queueFamilyProperties, []( auto const & qfp )
                        { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); } );

        uint32_t graphicsIndex = static_cast<uint32_t>( std::distance( queueFamilyProperties.begin(), graphicsQueueFamilyProperty ) );

        // determine a queueFamilyIndex that supports present
        // first check if the graphicsIndex is good enough
        uint32_t presentIndex = m_PhysicalDevice.getSurfaceSupportKHR( graphicsIndex, *m_Surface )
                            ? graphicsIndex
                            : static_cast<uint32_t>( queueFamilyProperties.size() );

        if ( presentIndex == queueFamilyProperties.size() )
        {
            // the graphicsIndex doesn't support present -> look for another family index that supports both
            // graphics and present
            for ( size_t i = 0; i < queueFamilyProperties.size(); i++ )
            {
                if ( ( queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics ) &&
                    m_PhysicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *m_Surface ) )
                {
                    graphicsIndex = static_cast<uint32_t>( i );
                    presentIndex  = graphicsIndex;
                    break;
                }
            }
            if ( presentIndex == queueFamilyProperties.size() )
            {
                // there's nothing like a single family index that supports both graphics and present -> look for another
                // family index that supports present
                for ( size_t i = 0; i < queueFamilyProperties.size(); i++ )
                {
                    if ( m_PhysicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *m_Surface ) )
                    {
                        presentIndex = static_cast<uint32_t>( i );
                        break;
                    }
                }
            }
        }
        if ( ( graphicsIndex == queueFamilyProperties.size() ) || ( presentIndex == queueFamilyProperties.size() ) )
        {
            LOG_CORE_ERROR("Vulkan: Could not find a queue for graphics or present!");
        }

        // query for Vulkan 1.3 features
        auto features = m_PhysicalDevice.getFeatures2();
        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
        vulkan13Features.dynamicRendering = vk::True;
        extendedDynamicStateFeatures.extendedDynamicState = vk::True;
        vulkan13Features.pNext = &extendedDynamicStateFeatures;
        features.pNext = &vulkan13Features;

        // create a Device
        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
        deviceQueueCreateInfo.queueFamilyIndex = graphicsIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.pNext = &features;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

        m_Device = vk::raii::Device( m_PhysicalDevice, deviceCreateInfo );
        m_GraphicsQueue = vk::raii::Queue( m_Device, graphicsIndex, 0 );
        m_PresentQueue = vk::raii::Queue( m_Device, presentIndex, 0 );
    }

    void VulkanGraphicsDevice::CreateSwapChain() {
        LOG_CORE_INFO("Vulkan: Creating Swap Chain...");

        // Get surface formats and extent
        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR( m_Surface );
        m_SwapChainSurfaceFormat = ChooseSwapSurfaceFormat(m_PhysicalDevice.getSurfaceFormatsKHR( m_Surface ));
        m_SwapChainExtent = ChooseSwapExtent(surfaceCapabilities);

        // Create swapchain
        vk::SwapchainCreateInfoKHR swapChainCreateInfo;
        swapChainCreateInfo.flags = vk::SwapchainCreateFlagsKHR();
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount = ChooseSwapMinImageCount(surfaceCapabilities);
        swapChainCreateInfo.imageFormat = m_SwapChainSurfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = m_SwapChainSurfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = m_SwapChainExtent;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
        swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapChainCreateInfo.presentMode = ChooseSwapPresentMode(m_PhysicalDevice.getSurfacePresentModesKHR( m_Surface ));
        swapChainCreateInfo.clipped = true;
        swapChainCreateInfo.oldSwapchain = nullptr;

        m_SwapChain = vk::raii::SwapchainKHR( m_Device, swapChainCreateInfo );
        m_SwapChainImages = m_SwapChain.getImages();
    }

    void VulkanGraphicsDevice::CreateImageViews() {
        LOG_CORE_INFO("Vulkan: Creating Image Views...");
        m_SwapChainImageViews.clear();

        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = m_SwapChainSurfaceFormat.format;
        imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        for (auto& image : m_SwapChainImages)
		{
			imageViewCreateInfo.image = image;
			m_SwapChainImageViews.emplace_back(m_Device, imageViewCreateInfo);
		}
    }

    void VulkanGraphicsDevice::CreateGraphicsPipeline() {
        LOG_CORE_INFO("Vulkan: Creating Graphics Pipeline...");
        
    }

    // Utility
    vk::SurfaceFormatKHR VulkanGraphicsDevice::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR VulkanGraphicsDevice::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                return availablePresentMode;
            }
        }
        return vk::PresentModeKHR::eFifo; // Garunteed
    }

    vk::Extent2D VulkanGraphicsDevice::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        int width, height;
        SDL_GetWindowSizeInPixels(m_Window, &width, &height);

        return {
            std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
    }

    uint32_t VulkanGraphicsDevice::ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities) {
        auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
		if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
		{
			minImageCount = surfaceCapabilities.maxImageCount;
		}
		return minImageCount;
    }

    // Resource Creation
    Scope<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return nullptr;
    }

    Scope<ITexture> VulkanGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return nullptr;
    }

    Scope<IShader> VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return nullptr;
    }

    Scope<IVertexArray> VulkanGraphicsDevice::CreateVertexArray(const VertexArrayDesc& desc) {
        return nullptr;
    }

    Scope<IPipelineState> VulkanGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return nullptr;
    }

    // Frame Management
    void VulkanGraphicsDevice::BeginFrame() {

    }

    void VulkanGraphicsDevice::EndFrame() {
        // Empty in OpenGL
    }

    void VulkanGraphicsDevice::Present() {

    }

    void VulkanGraphicsDevice::SetClearColor(Vec4 color) {

    }

    // Draw call
    void VulkanGraphicsDevice::SubmitDraw(uint32_t indexCount) {

    }

    // Resize
    void VulkanGraphicsDevice::Resize(int width, int height) {

    }

} // namespace Engine
