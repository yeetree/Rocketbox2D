#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"
#include <SDL3/SDL_vulkan.h>

VulkanContext::VulkanContext(SDL_Window* window) : m_Window(window) {
    CreateInstance();
    SetupDebugMessanger();
    CreateSurface();
    PickPhysicalDevice();
}

VulkanContext::~VulkanContext() {
    // Nothing
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL VulkanContext::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
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

void VulkanContext::CreateInstance() {
    LOG_CORE_INFO("Vulkan: Creating Vulkan instance...");

    // App info
    constexpr vk::ApplicationInfo appInfo(
        "VKTest",
        VK_MAKE_VERSION( 1, 0, 0 ),
        "Engine",
        VK_MAKE_VERSION( 1, 0, 0 ),
        vk::ApiVersion13
    );

    // Get the required layers
    std::vector<char const*> requiredLayers;
    if (k_EnableValidationLayers) {
        requiredLayers.assign(k_ValidationLayers.begin(), k_ValidationLayers.end());
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
    if (k_EnableValidationLayers) {
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

void VulkanContext::SetupDebugMessanger() {
    // Set up debug messanger
    if(!k_EnableValidationLayers) { return; }

    LOG_CORE_INFO("Vulkan: Setting up Debug Messanger...");

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError );
    vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
    debugUtilsMessengerCreateInfoEXT.messageSeverity = severityFlags;
    debugUtilsMessengerCreateInfoEXT.messageType = messageTypeFlags;
    debugUtilsMessengerCreateInfoEXT.pfnUserCallback = &DebugCallback;
    m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void VulkanContext::CreateSurface() {
    LOG_CORE_INFO("Vulkan: Creating surface...");

    // Thank god we use SDL
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(m_Window, *m_Instance, NULL, &surface)) {
        LOG_CORE_ERROR("Vulkan: Could not create surface: SDL: {0}", SDL_GetError());
        return; // Failure
    }
    m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
}

void VulkanContext::PickPhysicalDevice() {
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
            for (auto const & extension : k_DeviceExtensions) {
                auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) {return strcmp(ext.extensionName, extension) == 0;});
                found = found &&  extensionIter != extensions.end();
            }
            isSuitable = isSuitable && found;
            uint32_t queueFamilyIndex = static_cast<uint32_t>(std::distance(queueFamilies.begin(), qfpIter));
            bool presentSupport = device.getSurfaceSupportKHR(queueFamilyIndex, *m_Surface);
            isSuitable = isSuitable && presentSupport;
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

vk::raii::Instance& VulkanContext::GetInstance() {
    return m_Instance;
}

vk::raii::PhysicalDevice& VulkanContext::GetPhysicalDevice() {
    return m_PhysicalDevice;
}

vk::raii::SurfaceKHR& VulkanContext::GetSurface() {
    return m_Surface;
}

SDL_Window* VulkanContext::GetWindow() {
    return m_Window;
}