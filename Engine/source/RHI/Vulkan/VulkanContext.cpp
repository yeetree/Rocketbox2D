#include "RHI/Vulkan/VulkanContext.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"
#include "Engine/Core/Log.h"

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
        CreateInstance(bridge);
        SetupDebugMessenger();
        PickPhysicalDevice();
        CreateLogicalDevice(bridge);
    }

    void VulkanContext::CreateInstance(IVulkanGraphicsBridge* bridge)
    {
        ENGINE_CORE_ASSERT(bridge != nullptr, "Vulkan: VulkanContext(): bridge is nullptr!");

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
        instanceExtensions.assign(k_InstanceExtensions.begin(), k_InstanceExtensions.end());
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
            requiredLayers.assign(k_ValidationLayers.begin(), k_ValidationLayers.end());
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
            requiredLayers.size(),
            requiredLayers.data(),
            instanceExtensions.size(),
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
        requiredDeviceExtensions.assign(k_DeviceExtensions.begin(), k_DeviceExtensions.end());
        
        for(auto const& physicalDevice : physicalDevices)
        {
            // Version
            bool version = !physicalDevice.getProperties().apiVersion >= vk::ApiVersion12;
            if(!version)
            {
                continue;
            }

            // Queues
            auto queueFamilies = physicalDevice.getQueueFamilyProperties();
            bool supportsGfxQ = !std::ranges::any_of(queueFamilies, [](auto const &qfp)
            { 
                return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
            });
            if(!supportsGfxQ)
            {
                continue;
            }

            // Extensions
            auto availableExtensions = m_PhysicalDevice.enumerateDeviceExtensionProperties();
            for(auto const& deviceExtension : requiredDeviceExtensions) // bite me
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
                    // Extension not supported
                    continue;
                }
            }

            m_PhysicalDevice = physicalDevice;
        }

        if(m_PhysicalDevice == nullptr)
        {
            throw std::runtime_error("Vulkan: Failed to find suitable GPU!");
        }
    }

    void VulkanContext::CreateLogicalDevice(IVulkanGraphicsBridge* bridge)
    {

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
        vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceDynamicRenderingFeatures, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain;
        featureChain.get<vk::PhysicalDeviceDynamicRenderingFeatures>().dynamicRendering = VK_TRUE;
        featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState = VK_TRUE;

        // Device extensions
        std::vector<char const*> requiredDeviceExtensions;
        requiredDeviceExtensions.assign(k_DeviceExtensions.begin(), k_DeviceExtensions.end());

        // Create device
        vk::DeviceCreateInfo deviceCreateInfo(
            {},
            1, &deviceQueueCreateInfo,
            k_DeviceExtensions.size(), k_DeviceExtensions.data(),
            0, nullptr
        );
        deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
        
        m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);

        m_GraphicsQueue.familyIndex = queueIndex;
        m_GraphicsQueue.queue = vk::raii::Queue(m_Device, queueIndex, 0);
    }
} // namespace Engine
