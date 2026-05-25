#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"

#include "RHI/Vulkan/RHI/VulkanSwapChain.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge)
        : m_Bridge(std::move(bridge))
    {
        m_Context = CreateScope<VulkanContext>(m_Bridge.get());

        vk::FenceCreateInfo fenceInfo;
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        for(int i = 0; i < k_MaxFramesInFlight; i++)
        {
            m_Fences.emplace_back(m_Context->GetDevice(), fenceInfo);
        }
    };

    Scope<ISwapChain> VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        return CreateScope<VulkanSwapChain>(m_Context.get(), m_Bridge.get(), desc);
    }

    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {
        m_Context->GetDevice().waitForFences(*m_Fences[m_FrameIndex], VK_TRUE, UINT64_MAX);
        m_Context->GetDevice().resetFences(*m_Fences[m_FrameIndex]);
    }

    void VulkanGraphicsDevice::EndFrame()
    {
        vk::SubmitInfo finalSubmit{};
        m_Context->GetGraphicsQueue().queue.submit(finalSubmit, m_Fences[m_FrameIndex]);
        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    // Swapchain passees
    ICommandBuffer* VulkanGraphicsDevice::BeginSwapChainPass(Ref<ISwapChain> swapchain)
    {
        if(swapchain == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: BeginSwapChainPass: swapchain is nullptr!");
            return nullptr;
        }
        
        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain.get());

        vsc->AcquireNextImage(m_FrameIndex);

        VulkanCommandBuffer* cmd = new VulkanCommandBuffer(m_Context.get()); // TODO: Vulkan: small memory leak just for testing
        return cmd;
    }

    void VulkanGraphicsDevice::EndSwapChainPass(Ref<ISwapChain> swapchain, ICommandBuffer* cmd)
    {
        if(swapchain == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: EndSwapChainPass: swapchain is nullptr!");
            return;
        }

        if(cmd == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: EndSwapChainPass: cmd is nullptr!");
            return;
        }

        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain.get());
        VulkanCommandBuffer* vcmd = static_cast<VulkanCommandBuffer*>(cmd);

        vk::raii::Semaphore& presentComplete = vsc->GetPresentCompleteSemaphore(m_FrameIndex);
        vk::raii::Semaphore& renderFinished = vsc->GetRenderFinishedSemaphore(m_FrameIndex);

        vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &*presentComplete;
        submitInfo.pWaitDstStageMask = &waitDestinationStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*(vcmd->GetCommandBuffer());
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &*renderFinished;

        m_Context->GetGraphicsQueue().queue.submit(submitInfo, nullptr);

        vsc->Present(m_FrameIndex);
    }

    void VulkanGraphicsDevice::OnDestroy()
    {
        m_Context->GetDevice().waitIdle();
        m_Fences.clear();
    }

} // namespace Engine
