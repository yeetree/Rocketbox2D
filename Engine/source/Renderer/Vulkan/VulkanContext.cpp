#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include "Renderer/Vulkan/IVulkanGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VulkanContext::VulkanContext(Engine::IVulkanGraphicsBridge* graphicsBridge, Engine::IWindow* window) {
    ENGINE_CORE_ASSERT(graphicsBridge != nullptr, "Vulkan: VulkanContext(): graphicsBridge is nullptr!");
    ENGINE_CORE_ASSERT(window != nullptr, "Vulkan: VulkanContext(): window is nullptr!");
    CreateInstance(graphicsBridge);    
    SetupDebugMessanger();
    CreateSurface(graphicsBridge, window);
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

void VulkanContext::CreateInstance(Engine::IVulkanGraphicsBridge* graphicsBridge) {
    LOG_CORE_INFO("Vulkan: Creating Vulkan instance...");

    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

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
        throw std::runtime_error("Vulkan: One or more required layers is not supported!");
        return; // Failure
    }

    // Get required extensions for platform
    std::vector<const char*> extensions = graphicsBridge->GetRequiredExtensions();

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
            throw std::runtime_error(std::format("Vulkan: Required extension not supported: {0}", std::string(extensions[i])));
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

    VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_Instance, vkGetInstanceProcAddr);
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

void VulkanContext::CreateSurface(Engine::IVulkanGraphicsBridge* graphicsBridge, Engine::IWindow* window) {
    LOG_CORE_INFO("Vulkan: Creating surface...");

    VkSurfaceKHR surface = graphicsBridge->CreateSurface(*m_Instance, window);
    m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
}

void VulkanContext::PickPhysicalDevice() {
    LOG_CORE_INFO("Vulkan: Selecting Physical Device...");
        
    // Select physical device
    std::vector<vk::raii::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        throw std::runtime_error("Vulkan: Failed to find GPUs with Vulkan support!");
        return; // Failure
    }

    // Loop through devices
    LOG_CORE_INFO("Vulkan: Found {0} physical device(s).", devices.size());

    for (const auto& device : devices) {
        vk::PhysicalDeviceProperties props = device.getProperties();
        LOG_CORE_INFO("Vulkan: Checking Device: {0}", (const char*)props.deviceName);

        // Check version
        if (props.apiVersion < VK_API_VERSION_1_3) {
            uint32_t major = VK_API_VERSION_MAJOR(props.apiVersion);
            uint32_t minor = VK_API_VERSION_MINOR(props.apiVersion);
            uint32_t patch = VK_API_VERSION_PATCH(props.apiVersion);
            LOG_CORE_WARN("  - Skipped: Minimum required API version: 1.3. Device API version: {0}.{1}.{2}", major, minor, patch);
            continue;
        }

        // Check queue support
        auto queueFamilies = device.getQueueFamilyProperties(); 

        auto it = std::ranges::find_if(queueFamilies, [](const vk::QueueFamilyProperties& qfp) {
            return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) ? true : false;
        });

        if (it == queueFamilies.end()) {
            LOG_CORE_WARN("  - Skipped: No Graphics queue support.");
            continue;
        }

        uint32_t graphicsIdx = static_cast<uint32_t>(std::distance(queueFamilies.begin(), it));

        // Extension Check
        auto availableExtensions = device.enumerateDeviceExtensionProperties();
        bool extensionsFound = true;
        for (const char* required : k_DeviceExtensions) {
            auto it = std::ranges::find_if(availableExtensions, [&](const auto& ext) {
                return strcmp(ext.extensionName, required) == 0;
            });
            if (it == availableExtensions.end()) {
                LOG_CORE_WARN("  - Skipped: Missing extension {0}", required);
                extensionsFound = false;
                break;
            }
        }
        if (!extensionsFound) continue;

        // surface support Check
        if (!device.getSurfaceSupportKHR(graphicsIdx, *m_Surface)) {
            LOG_CORE_WARN("  - Skipped: Queue family {0} does not support Presentation.", graphicsIdx);
            continue;
        }

        m_PhysicalDevice = device;
        LOG_CORE_INFO("Vulkan: Selected GPU: {0}", (const char*)props.deviceName);
        return; 
    }
    throw std::runtime_error("Vulkan: Failed to find suitable GPU!");
    return; // Failure
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