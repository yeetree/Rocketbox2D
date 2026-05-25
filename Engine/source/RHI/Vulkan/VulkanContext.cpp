#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Debug callback

static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    switch(severity)
    {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        {
            LOG_CORE_WARN("Vulkan: {0}: {1}", to_string(type), pCallbackData->pMessage);
            break;
        }
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        {
            LOG_CORE_ERROR("Vulkan: {0}: {1}", to_string(type), pCallbackData->pMessage);
            break;
        }
    }

    return vk::False;
}

namespace Engine
{
    VulkanContext::VulkanContext(IVulkanGraphicsBridge* bridge)
    {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        CreateInstance(bridge);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_Instance);
        SetupDebugMessenger();
        PickPhysicalDevice();
        CreateLogicalDevice(bridge);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(*m_Device);
        CreateCommandPool();
    }

    VulkanContext::~VulkanContext()
    {
        if(m_Device != nullptr)
        {
            m_Device.waitIdle();
        }

        if(m_Allocator)
        {
            vmaDestroyAllocator(m_Allocator);
        }
    }

    void VulkanContext::CreateInstance(IVulkanGraphicsBridge* bridge)
    {
        ENGINE_CORE_ASSERT(bridge != nullptr, "Vulkan: VulkanContext(): bridge is nullptr!");
        LOG_CORE_INFO("Vulkan: Creating instance...");

        // Create instance
        // TODO: Vulkan: Get application info from here
        constexpr vk::ApplicationInfo appInfo(
            "Engine App",
            VK_MAKE_VERSION(1, 0, 0),
            "Engine",
            VK_MAKE_VERSION(1, 0, 0),
            vk::ApiVersion12
        );

        // Get instance extensions and make sure they're all supported
        auto instanceExtensions = bridge->GetInstanceExtensions();
        // Add engine instance extensions
        instanceExtensions.insert(instanceExtensions.end(), k_InstanceExtensions.begin(), k_InstanceExtensions.end());
        auto extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for(auto const& ext : instanceExtensions)
        {
            if (
                std::ranges::none_of(
                    extensionProperties,
                    [&ext](auto const& extensionProperty)
                    {
                        return strcmp(extensionProperty.extensionName, ext) == 0;
                    }
                )
            )
            {
                throw std::runtime_error(std::format("Required extension not supported: {0}", ext));
            }
        }

        // Get required layers and make sure they're all supported
        std::vector<char const*> requiredLayers;
        if (k_EnableValidationLayers)
        {
            requiredLayers.insert(requiredLayers.end(), k_ValidationLayers.begin(), k_ValidationLayers.end());
        }

        // Check if the required layers are supported by the Vulkan implementation.
        auto layerProperties = m_Context.enumerateInstanceLayerProperties();
        for(auto const& layer : requiredLayers)
        {
            if (
                std::ranges::none_of(
                    layerProperties,
                    [&layer](auto const& layerProperty)
                    {
                        return strcmp(layerProperty.layerName, layer) == 0;
                    }
                )
            )
            {
                throw std::runtime_error(std::format("Required layer not supported: {0}", layer));
            }
        }

        vk::InstanceCreateInfo createInfo{
            {},
            &appInfo,
            (uint32_t)requiredLayers.size(),
            requiredLayers.data(),
            (uint32_t)instanceExtensions.size(),
            instanceExtensions.data()
        };

        m_Instance = vk::raii::Instance(m_Context, createInfo);
    }

    void VulkanContext::SetupDebugMessenger()
    {
        if(!k_EnableValidationLayers)
        {
            return;
        }

        // Create debug messanger and attach callback
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        );

        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
        );

        vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT(
            {},
            severityFlags,
            messageTypeFlags,
            &DebugCallback
        );
        
        m_DebugMessenger = m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
    }

    void VulkanContext::PickPhysicalDevice()
    {
        LOG_CORE_INFO("Vulkan: Selecting physical device...");
        // Loop through physical GPUs and find worthy devices
        auto physicalDevices = m_Instance.enumeratePhysicalDevices();
        if (physicalDevices.empty())
        {
            throw std::runtime_error("Vulkan: Failed to find GPUs with Vulkan support!");
        }

        // TODO: Vulkan: More sophisticated device selection

        // Make sure physical device meets all of our requirements:
        //      Vulkan 1.2
        //      Graphics queue
        //      k_DeviceExtensions

        std::vector<const char*> requiredDeviceExtensions;
        requiredDeviceExtensions.insert(requiredDeviceExtensions.end(), k_DeviceExtensions.begin(), k_DeviceExtensions.end());
        
        for(auto const& physicalDevice : physicalDevices)
        {
            LOG_CORE_INFO("Vulkan: Trying: {0}", std::string(physicalDevice.getProperties().deviceName));
            // Version
            bool version = physicalDevice.getProperties().apiVersion >= vk::ApiVersion12;
            if(!version)
            {
                LOG_CORE_WARN("Vulkan: - Skipped: Minimum Vulkan version not supported.");
                continue;
            }

            // Queues
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            bool supportsGfxQ = std::ranges::any_of(queueFamilies, [](auto const &qfp)
            { 
                return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
            });
            if(!supportsGfxQ)
            {
                LOG_CORE_WARN("Vulkan: - Skipped: Does not have graphics queue support.");
                continue;
            }

            // Extensions
            auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
            bool allExtensionsSupported = true;
            for(auto const& deviceExtension : requiredDeviceExtensions)
            {
                if (
                    std::ranges::none_of(
                        availableExtensions,
                        [&deviceExtension](auto const& avaliableExtension)
                        {
                            return strcmp(avaliableExtension.extensionName, deviceExtension) == 0;
                        }
                    )
                )
                {
                    LOG_CORE_WARN("Vulkan: - Extension not supported: {0}.", deviceExtension);
                    allExtensionsSupported = false;
                    break;
                }
            }

            if(!allExtensionsSupported)
            {
                LOG_CORE_WARN("Vulkan: - Skipped: Does not support all extensions.");
                continue;
            }

            LOG_CORE_INFO("Vulkan: Selected: {0}", std::string(physicalDevice.getProperties().deviceName));
            m_PhysicalDevice = physicalDevice;
            break;
        }

        if(m_PhysicalDevice == nullptr)
        {
            throw std::runtime_error("Vulkan: Failed to find suitable GPU!");
        }
    }

    void VulkanContext::CreateLogicalDevice(IVulkanGraphicsBridge* bridge)
    {
        LOG_CORE_INFO("Vulkan: Creating logical device...");
        // TODO: Vulkan: Pick separate graphics and presentation queues
        // TODO: Vulkan: Make feature selection less trash

        // Create dummy surface
        VkSurfaceKHR* surf = bridge->CreateDummySurface(*m_Instance);
        if (surf == nullptr)
        {
            throw std::runtime_error("Vulkan: Dummy surface is invalid!");
        }

        // Get graphics queue
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();
        uint32_t queueIndex = ~0;
        for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
        {
        if (
            (queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            m_PhysicalDevice.getSurfaceSupportKHR(qfpIndex, *surf))
        {
            // found a queue family that supports both graphics and present
            queueIndex = qfpIndex;
            break;
        }
        }
        if (queueIndex == ~0)
        {
            throw std::runtime_error("Vulkan: Could not find a queue for both graphics and presentation!");
        }

        bridge->DestroyDummySurface(*m_Instance);

        // Device queues
        float queuePriority = 0.5f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
            {},
            queueIndex,
            1, &queuePriority
        );

        // Get features
        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceSynchronization2Features, vk::PhysicalDeviceDynamicRenderingFeatures, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
        featureChain.get<vk::PhysicalDeviceDynamicRenderingFeatures>().dynamicRendering = VK_TRUE;
        featureChain.get<vk::PhysicalDeviceSynchronization2Features>().synchronization2 = VK_TRUE;
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = VK_TRUE;

        // Device extensions
        std::vector<char const*> requiredDeviceExtensions;
        requiredDeviceExtensions.assign(k_DeviceExtensions.begin(), k_DeviceExtensions.end());

        // Create device
        vk::DeviceCreateInfo deviceCreateInfo(
            {},
            1, &deviceQueueCreateInfo,
            0, nullptr,
            k_DeviceExtensions.size(), k_DeviceExtensions.data()
        );

        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        
        m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);

        m_GraphicsQueue.familyIndex = queueIndex;
        m_GraphicsQueue.queue = vk::raii::Queue(m_Device, queueIndex, 0);
    }

    void VulkanContext::CreateAllocator()
    {
        LOG_CORE_INFO("Vulkan: Creating allocator...");

        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice = *m_PhysicalDevice;
        allocatorCreateInfo.device = *m_Device;
        allocatorCreateInfo.instance = *m_Instance;
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
        
        vmaCreateAllocator(&allocatorCreateInfo, &m_Allocator);
    }

    void VulkanContext::CreateCommandPool()
    {
        LOG_CORE_INFO("Vulkan: Creating command pool...");

        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            m_GraphicsQueue.familyIndex
        );

        m_CommandPool = vk::raii::CommandPool(m_Device, poolInfo);
    };
} // namespace Engine
