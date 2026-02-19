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

void VulkanDevice::CreateAllocator(VulkanContext& context) {
    // Setup VMA allocator
    // We're configuring VMA for Vulkan 1.3 because 1.4 is scary :(
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorInfo.physicalDevice = *context.GetPhysicalDevice();
    allocatorInfo.device = *m_Device;
    allocatorInfo.instance = *context.GetInstance();

    // Setup vulkanFunctions for VMA allocator
    auto &instance = context.GetInstance();
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)instance.getProcAddr("vkGetInstanceProcAddr");
    vulkanFunctions.vkGetDeviceProcAddr   = (PFN_vkGetDeviceProcAddr)m_Device.getProcAddr("vkGetDeviceProcAddr");

    vulkanFunctions.vkGetPhysicalDeviceProperties       = (PFN_vkGetPhysicalDeviceProperties)instance.getProcAddr("vkGetPhysicalDeviceProperties");
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instance.getProcAddr("vkGetPhysicalDeviceMemoryProperties");
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = (PFN_vkGetPhysicalDeviceMemoryProperties2)instance.getProcAddr("vkGetPhysicalDeviceMemoryProperties2");

    vulkanFunctions.vkAllocateMemory                    = (PFN_vkAllocateMemory)m_Device.getProcAddr("vkAllocateMemory");
    vulkanFunctions.vkFreeMemory                        = (PFN_vkFreeMemory)m_Device.getProcAddr("vkFreeMemory");
    vulkanFunctions.vkMapMemory                         = (PFN_vkMapMemory)m_Device.getProcAddr("vkMapMemory");
    vulkanFunctions.vkUnmapMemory                       = (PFN_vkUnmapMemory)m_Device.getProcAddr("vkUnmapMemory");
    vulkanFunctions.vkFlushMappedMemoryRanges           = (PFN_vkFlushMappedMemoryRanges)m_Device.getProcAddr("vkFlushMappedMemoryRanges");
    vulkanFunctions.vkInvalidateMappedMemoryRanges      = (PFN_vkInvalidateMappedMemoryRanges)m_Device.getProcAddr("vkInvalidateMappedMemoryRanges");
    vulkanFunctions.vkBindBufferMemory                  = (PFN_vkBindBufferMemory)m_Device.getProcAddr("vkBindBufferMemory");
    vulkanFunctions.vkBindImageMemory                   = (PFN_vkBindImageMemory)m_Device.getProcAddr("vkBindImageMemory");
    vulkanFunctions.vkGetBufferMemoryRequirements       = (PFN_vkGetBufferMemoryRequirements)m_Device.getProcAddr("vkGetBufferMemoryRequirements");
    vulkanFunctions.vkGetImageMemoryRequirements        = (PFN_vkGetImageMemoryRequirements)m_Device.getProcAddr("vkGetImageMemoryRequirements");
    vulkanFunctions.vkCreateBuffer                      = (PFN_vkCreateBuffer)m_Device.getProcAddr("vkCreateBuffer");
    vulkanFunctions.vkDestroyBuffer                     = (PFN_vkDestroyBuffer)m_Device.getProcAddr("vkDestroyBuffer");
    vulkanFunctions.vkCreateImage                       = (PFN_vkCreateImage)m_Device.getProcAddr("vkCreateImage");
    vulkanFunctions.vkDestroyImage                      = (PFN_vkDestroyImage)m_Device.getProcAddr("vkDestroyImage");
    vulkanFunctions.vkCmdCopyBuffer                     = (PFN_vkCmdCopyBuffer)m_Device.getProcAddr("vkCmdCopyBuffer");
    vulkanFunctions.vkGetDeviceBufferMemoryRequirements = (PFN_vkGetDeviceBufferMemoryRequirements)m_Device.getProcAddr("vkGetDeviceBufferMemoryRequirements");
    vulkanFunctions.vkGetDeviceImageMemoryRequirements = (PFN_vkGetDeviceImageMemoryRequirements)m_Device.getProcAddr("vkGetDeviceImageMemoryRequirements");
    vulkanFunctions.vkGetBufferMemoryRequirements2KHR = (PFN_vkGetBufferMemoryRequirements2)m_Device.getProcAddr("vkGetBufferMemoryRequirements2");
    vulkanFunctions.vkGetImageMemoryRequirements2KHR  = (PFN_vkGetImageMemoryRequirements2)m_Device.getProcAddr("vkGetImageMemoryRequirements2");
    vulkanFunctions.vkBindBufferMemory2KHR            = (PFN_vkBindBufferMemory2)m_Device.getProcAddr("vkBindBufferMemory2");
    vulkanFunctions.vkBindImageMemory2KHR             = (PFN_vkBindImageMemory2)m_Device.getProcAddr("vkBindImageMemory2");

    allocatorInfo.pVulkanFunctions = &vulkanFunctions;
    
    if (vmaCreateAllocator(&allocatorInfo, &m_Allocator) != VK_SUCCESS) {
        LOG_CORE_ERROR("Vulkan: Failed to create VMA Allocator!");
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