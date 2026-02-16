#include "Renderer/Vulkan/VulkanFrame.h"
#include "Renderer/Vulkan/VulkanDevice.h"
#include "Renderer/Vulkan/VulkanConstants.h"
#include "Engine/Core/Log.h"

VulkanFrame::VulkanFrame(VulkanDevice& device) {
    CreateCommandBuffer(device);
    CreateSyncObjects(device);
}

VulkanFrame::~VulkanFrame() {
    // Nothing
}

void VulkanFrame::CreateCommandBuffer(VulkanDevice& device) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = device.GetCommandPool();
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;
    auto commandBuffers = device.GetDevice().allocateCommandBuffers( allocInfo );
    m_CommandBuffer = std::move( commandBuffers[0] );
}

void VulkanFrame::CreateSyncObjects(VulkanDevice& device) {
    m_RenderFinishedSemaphore = vk::raii::Semaphore(device.GetDevice(), vk::SemaphoreCreateInfo());
    m_ImageAvailableSemaphore = vk::raii::Semaphore(device.GetDevice(), vk::SemaphoreCreateInfo());
    m_InFlightFence = vk::raii::Fence(device.GetDevice(), vk::FenceCreateInfo{vk::FenceCreateFlagBits::eSignaled});
}

vk::raii::Semaphore& VulkanFrame::GetImageAvailableSemaphore() {
    return m_ImageAvailableSemaphore;
}

vk::raii::Semaphore& VulkanFrame::GetRenderFinishedSemaphore() {
    return m_RenderFinishedSemaphore;
}

vk::raii::Fence& VulkanFrame::GetInFlightFence() {
    return m_InFlightFence;
}

vk::raii::CommandBuffer& VulkanFrame::GetCommandBuffer() {
    return m_CommandBuffer;
}