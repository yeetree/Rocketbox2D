#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"

#include "RHI/Vulkan/RHI/VulkanSwapChain.h"
#include "RHI/Vulkan/RHI/VulkanShader.h"
#include "RHI/Vulkan/RHI/VulkanPipeline.h"
#include "RHI/Vulkan/RHI/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanConstants.h"

#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge)
        : m_Bridge(std::move(bridge))
    {
        m_Context = CreateScope<VulkanContext>(m_Bridge.get());

        // Create one frame per frame in flight
        for(int i = 0; i < k_MaxFramesInFlight; i++)
        {
            Scope<VulkanFrame> frame = CreateScope<VulkanFrame>(m_Context.get());
            m_Frames.push_back(std::move(frame));
        }
    };

    Scope<ISwapChain> VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        return CreateScope<VulkanSwapChain>(m_Context.get(), m_Bridge.get(), desc);
    }

    Scope<IShader> VulkanGraphicsDevice::CreateShader(const ShaderDesc& desc)
    {
        return CreateScope<VulkanShader>(m_Context.get(), desc);
    }

    Scope<IPipeline> VulkanGraphicsDevice::CreatePipeline(const PipelineDesc& desc)
    {
        return CreateScope<VulkanPipeline>(m_Context.get(), desc);
    }


    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {
        m_Context->GetDevice().waitForFences(*m_Frames[m_FrameIndex]->GetFence(), VK_TRUE, UINT64_MAX);
        m_Context->GetDevice().resetFences(*m_Frames[m_FrameIndex]->GetFence());
        m_Frames[m_FrameIndex]->Reset();
    }

    void VulkanGraphicsDevice::EndFrame()
    {
        vk::SubmitInfo finalSubmit{};
        m_Context->GetGraphicsQueue().queue.submit(finalSubmit, m_Frames[m_FrameIndex]->GetFence());
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

        return m_Frames[m_FrameIndex]->GetCommandBuffer(m_Context.get());
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
        vk::raii::Semaphore& renderFinished = vsc->GetRenderFinishedSemaphore();

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
        m_Frames.clear();
    }

} // namespace Engine
