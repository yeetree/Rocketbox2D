#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Engine
{
    VulkanContext::VulkanContext(Ref<IVulkanGraphicsBridge> bridge, Ref<IWindow> window) : m_Bridge(bridge)
    {
        ENGINE_CORE_ASSERT(m_Bridge != nullptr, "Vulkan: VulkanContext(): bridge is nullptr!");
        CreateInstance();
        CreateSurface(window);
        PickPhysicalDevice();
        CreateLogicalDevice();
        SetupDebugMessanger();
    }

    VulkanContext::~VulkanContext()
    {
        if (*m_Device) {
            m_Device.waitIdle();
        }
        // m_Allocator is not raii
        //if (m_Allocator) {
        //    vmaDestroyAllocator(m_Allocator);
        //    m_Allocator = nullptr;
        //}
        m_Bridge->DestroySurface(*m_Instance, *m_Surface);
        m_Surface.clear();
    }

    void VulkanContext::CreateSurface(Ref<IWindow> window)
    {
        LOG_CORE_INFO("Vulkan: Creating surface...");

        VkSurfaceKHR surface = m_Bridge->CreateSurface(*m_Instance, window);
        m_Surface = vk::raii::SurfaceKHR(m_Instance, surface);
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

    void VulkanContext::CreateInstance()
    {
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
        std::vector<const char*> extensions = m_Bridge->GetInstanceExtensions();

        // Add Engine extensions
        if (k_EnableValidationLayers) {
            extensions.push_back(vk::EXTDebugUtilsExtensionName);
        }

        std::vector<vk::ExtensionProperties> extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (uint32_t i = 0; i < extensions.size(); ++i)
        {
            if (std::ranges::none_of(extensionProperties,
                                    [extension = extensions[i]](auto const& extensionProperty)
                                    { return strcmp(extensionProperty.extensionName, extension) == 0; }))
            {
                throw std::runtime_error(std::format("Vulkan: Required extension not supported: {0}", std::string(extensions[i])));
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

    void VulkanContext::SetupDebugMessanger()
    {
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

    void VulkanContext::PickPhysicalDevice()
    {
        LOG_CORE_INFO("Vulkan: Selecting Physical Device...");
        
        // Select physical device
        std::vector<vk::raii::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();
        if (devices.empty()) {
            throw std::runtime_error("Vulkan: Failed to find GPUs with Vulkan support!");
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
            if (!device.getSurfaceSupportKHR(graphicsIdx, m_Surface)) {
                LOG_CORE_WARN("  - Skipped: Queue family {0} does not support Presentation.", graphicsIdx);
                continue;
            }

            m_PhysicalDevice = device;
            LOG_CORE_INFO("Vulkan: Selected GPU: {0}", (const char*)props.deviceName);
            return; 
        }
        throw std::runtime_error("Vulkan: Failed to find suitable GPU!");
    }

    void VulkanContext::CreateLogicalDevice()
    {
        LOG_CORE_INFO("Vulkan: Creating Logical Device...");

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

        // get the first index into queueFamilyProperties which supports both graphics and present
        for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
        {
            if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
                m_PhysicalDevice.getSurfaceSupportKHR(qfpIndex, m_Surface))
            {
                // found a queue family that supports both graphics and present
                m_GraphicsQueue.familyIndex = qfpIndex;
                break;
            }
        }
        if (m_GraphicsQueue.familyIndex == -1)
        {
            throw std::runtime_error("Vulkan: CreateLogicalDevice(): Could not find a queue for graphics and present!");
        }

        // query for Vulkan 1.3 features
        vk::PhysicalDeviceVulkan11Features vulkan11Features;
        vulkan11Features.shaderDrawParameters = true;
        
        vk::PhysicalDeviceVulkan13Features vulkan13Features;
        vulkan13Features.dynamicRendering = true;
        vulkan13Features.synchronization2 = true;
        
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
        extendedDynamicStateFeatures.extendedDynamicState = true;

        vk::StructureChain<vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan11Features,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain(
            vk::PhysicalDeviceFeatures2{},
            vulkan11Features,
            vulkan13Features,
            extendedDynamicStateFeatures
        );

        // create a Device
        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
        deviceQueueCreateInfo.queueFamilyIndex = m_GraphicsQueue.familyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(k_DeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = k_DeviceExtensions.data();

        m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);
        m_GraphicsQueue.queue = vk::raii::Queue(m_Device, m_GraphicsQueue.familyIndex, 0);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_Instance, vkGetInstanceProcAddr, *m_Device, vkGetDeviceProcAddr);
    }
} // namespace Engine
