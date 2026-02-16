#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"

VulkanDevice::VulkanDevice(VulkanContext& context) {
    CreateLogicalDevice(context);
    CreateCommandPool();
}

VulkanDevice::~VulkanDevice() {
    if (*m_Device) {
        m_Device.waitIdle();
    }
}


void VulkanDevice::CreateLogicalDevice(VulkanContext& context) {
    LOG_CORE_INFO("Vulkan: Creating Logical Device...");

    
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = context.GetPhysicalDevice().getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both graphics and present
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
    {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            context.GetPhysicalDevice().getSurfaceSupportKHR(qfpIndex, context.GetSurface()))
        {
            // found a queue family that supports both graphics and present
            m_QueueIndex= qfpIndex;
            break;
        }
    }
    if (m_QueueIndex == ~0)
    {
        LOG_CORE_ERROR("Vulkan: Could not find a queue for graphics and present!");
        return; // Failure
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
    deviceQueueCreateInfo.queueFamilyIndex = m_QueueIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>();
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(k_DeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = k_DeviceExtensions.data();

    m_Device = vk::raii::Device(context.GetPhysicalDevice(), deviceCreateInfo);
    m_Queue = vk::raii::Queue(m_Device, m_QueueIndex, 0);
}

void VulkanDevice::CreateCommandPool() {
    LOG_CORE_INFO("Vulkan: Creating Command Pool...");
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_QueueIndex;
    m_CommandPool = vk::raii::CommandPool(m_Device, poolInfo);
}

vk::raii::Device& VulkanDevice::GetDevice() {
    return m_Device;
}

vk::raii::Queue& VulkanDevice::GetQueue() {
    return m_Queue;
}

vk::raii::CommandPool& VulkanDevice::GetCommandPool() {
    return m_CommandPool;
}