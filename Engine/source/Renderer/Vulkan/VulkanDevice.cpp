#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanContext.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"

VulkanDevice::VulkanDevice(VulkanContext& context) {
    CreateLogicalDevice(context);
    CreateCommandPool();
    CreateAllocator(context);
}

VulkanDevice::~VulkanDevice() {
    if (*m_Device) {
        m_Device.waitIdle();
    }
    // m_Allocator is not raii
    if (m_Allocator) {
        vmaDestroyAllocator(m_Allocator);
        m_Allocator = nullptr;
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
        throw std::runtime_error("Vulkan: Could not find a queue for graphics and present!");
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

    VULKAN_HPP_DEFAULT_DISPATCHER.init(*context.GetInstance(), vkGetInstanceProcAddr, *m_Device, vkGetDeviceProcAddr);
}

void VulkanDevice::CreateCommandPool() {
    LOG_CORE_INFO("Vulkan: Creating Command Pool...");
    vk::CommandPoolCreateInfo poolInfo;
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    poolInfo.queueFamilyIndex = m_QueueIndex;
    m_CommandPool = vk::raii::CommandPool(m_Device, poolInfo);
}

void VulkanDevice::CreateAllocator(VulkanContext& context) {
    // Setup VMA allocator
    // We're configuring VMA for Vulkan 1.3 because 1.4 is scary :(

    // Setup vulkanFunctions for VMA allocator
    auto &instance = context.GetInstance();
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr   = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
    vulkanFunctions.vkGetPhysicalDeviceProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties;
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties;
    vulkanFunctions.vkAllocateMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkAllocateMemory;
    vulkanFunctions.vkFreeMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkFreeMemory;
    vulkanFunctions.vkMapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkMapMemory;
    vulkanFunctions.vkUnmapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkUnmapMemory;
    vulkanFunctions.vkFlushMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkFlushMappedMemoryRanges;
    vulkanFunctions.vkInvalidateMappedMemoryRanges = VULKAN_HPP_DEFAULT_DISPATCHER.vkInvalidateMappedMemoryRanges;
    vulkanFunctions.vkBindBufferMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory;
    vulkanFunctions.vkBindImageMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory;
    vulkanFunctions.vkGetBufferMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements;
    vulkanFunctions.vkGetImageMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements;
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties2;
    vulkanFunctions.vkGetBufferMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements2;
    vulkanFunctions.vkGetImageMemoryRequirements2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements2;
    vulkanFunctions.vkBindBufferMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory2;
    vulkanFunctions.vkBindImageMemory2KHR = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory2;
    vulkanFunctions.vkGetDeviceBufferMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceBufferMemoryRequirements;
    vulkanFunctions.vkGetDeviceImageMemoryRequirements = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceImageMemoryRequirements;
    vulkanFunctions.vkCreateBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateBuffer;
    vulkanFunctions.vkDestroyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyBuffer;
    vulkanFunctions.vkCreateImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateImage;
    vulkanFunctions.vkDestroyImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyImage;
    vulkanFunctions.vkCmdCopyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdCopyBuffer;

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorInfo.physicalDevice   = *context.GetPhysicalDevice();
    allocatorInfo.device           = *m_Device;
    allocatorInfo.instance         = *context.GetInstance();
    allocatorInfo.pVulkanFunctions = &vulkanFunctions;
    
    if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS) {
        throw std::runtime_error("Vulkan: Failed to create VMA Allocator!");
    }
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

VmaAllocator& VulkanDevice::GetAllocator() {
    return m_Allocator;
}