#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"

#include "RHI/Vulkan/RHI/VulkanSwapChain.h"
#include "RHI/Vulkan/RHI/VulkanShader.h"
#include "RHI/Vulkan/RHI/VulkanPipeline.h"
#include "RHI/Vulkan/RHI/VulkanBuffer.h"
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

        // Single-time command pool
        vk::CommandPoolCreateInfo poolInfo(
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            m_Context->GetGraphicsQueue().familyIndex
        );
        m_SingleCommandPool = vk::raii::CommandPool(m_Context->GetDevice(), poolInfo);
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

    Scope<IBuffer> VulkanGraphicsDevice::CreateBuffer(const BufferDesc& desc)
    {
        return CreateScope<VulkanBuffer>(m_Context.get(), desc);
    }

    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {
        m_Context->GetDevice().waitForFences(*m_Frames[m_FrameIndex]->GetFence(), VK_TRUE, UINT64_MAX);
        m_Context->GetDevice().resetFences(*m_Frames[m_FrameIndex]->GetFence());
        m_FrameSubmits.clear();
        m_FrameCommandBuffers.clear();
        m_FrameWaitSemaphores.clear();
        m_FrameSignalSemaphores.clear();
        m_FrameStageFlags.clear();
        m_FrameSwapChainPresentations.clear();
        m_Frames[m_FrameIndex]->Reset();
    }

    void VulkanGraphicsDevice::EndFrame()
    {
        uint32_t submitCount = static_cast<uint32_t>(m_FrameSubmits.size());
        const vk::SubmitInfo* finalSubmit = submitCount > 0 ? m_FrameSubmits.data() : nullptr;

        m_Context->GetGraphicsQueue().queue.submit(
            vk::ArrayProxy<const vk::SubmitInfo>(submitCount, finalSubmit), 
            *m_Frames[m_FrameIndex]->GetFence()
        );

        for(auto const& swapchain : m_FrameSwapChainPresentations)
        {
            swapchain->Present(m_Context.get(), m_FrameIndex);
        }

        m_FrameIndex = (m_FrameIndex + 1) % k_MaxFramesInFlight;
    }

    // Single time commands
    ICommandBuffer* VulkanGraphicsDevice::BeginSingleTimeCommands()
    {
        return new VulkanCommandBuffer(m_Context.get(), *m_SingleCommandPool);
    }

    void VulkanGraphicsDevice::EndSingleTimeCommands(ICommandBuffer* cmd)
    {
        if(cmd == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: EndSingleTimeCommands: cmd is nullptr!");
            return;
        }

        VulkanCommandBuffer* vcmd = static_cast<VulkanCommandBuffer*>(cmd);

        vk::SubmitInfo submitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &*(vcmd->GetCommandBuffer());

        vk::FenceCreateInfo fenceInfo{};
        
        vk::raii::Fence singleTimeFence = vk::raii::Fence(m_Context->GetDevice(), fenceInfo);

        m_Context->GetGraphicsQueue().queue.submit(submitInfo, *singleTimeFence);

        m_Context->GetDevice().waitForFences(*singleTimeFence, VK_TRUE, UINT64_MAX);
        m_Context->GetDevice().resetFences(*singleTimeFence);

        vcmd->FreeStagingBufferAllocations();
        delete vcmd;
    }

    // Swapchain passees
    ICommandBuffer* VulkanGraphicsDevice::BeginSwapChainPass(ISwapChain* swapchain)
    {
        if(swapchain == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: BeginSwapChainPass: swapchain is nullptr!");
            return nullptr;
        }
        
        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain);

        vsc->AcquireNextImage(m_Context.get(), m_FrameIndex);

        VulkanCommandBuffer* vcmd = m_Frames[m_FrameIndex]->GetCommandBufferPool()->GetCommandBuffer(m_Context.get());
        vcmd->FreeStagingBufferAllocations();
        vcmd->SetFrameInfo(m_FrameIndex, m_Frames[m_FrameIndex].get());
        return vcmd;
    }

    void VulkanGraphicsDevice::EndSwapChainPass(ISwapChain* swapchain, ICommandBuffer* cmd)
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

        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain);
        VulkanCommandBuffer* vcmd = static_cast<VulkanCommandBuffer*>(cmd);

        vk::Semaphore presentComplete = *vsc->GetPresentCompleteSemaphore(m_FrameIndex);
        vk::Semaphore renderFinished  = *vsc->GetRenderFinishedSemaphore();

        // Save frame submission info
        m_FrameCommandBuffers.push_back(*(vcmd->GetCommandBuffer()));
        m_FrameWaitSemaphores.push_back(presentComplete);
        m_FrameSignalSemaphores.push_back(renderFinished);
        m_FrameStageFlags.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);

        // Submit info
        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = &m_FrameWaitSemaphores.back();
        submitInfo.pWaitDstStageMask    = &m_FrameStageFlags.back();
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &m_FrameCommandBuffers.back();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores     = &m_FrameSignalSemaphores.back();

        // Push submission and presentation
        m_FrameSubmits.push_back(submitInfo);
        m_FrameSwapChainPresentations.push_back(vsc);
    }

    void VulkanGraphicsDevice::ResizeSwapChain(ISwapChain* swapchain, uint32_t width, uint32_t height)
    {
        if(swapchain == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: ResizeSwapChain: swapchain is nullptr!");
            return;
        }

        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain);

        vsc->Resize(width, height);
    }

    void VulkanGraphicsDevice::SetSwapChainPresentation(ISwapChain* swapchain, PresentMode presentation)
    {
        if(swapchain == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: SetSwapChainPresentation: swapchain is nullptr!");
            return;
        }

        VulkanSwapChain* vsc = static_cast<VulkanSwapChain*>(swapchain);

        vsc->SetPresentation(presentation);
    }

    void VulkanGraphicsDevice::SetBufferData(IBuffer* buffer, void* data, size_t size)
    {
        if(buffer == nullptr)
        {
            LOG_CORE_ERROR("Vulkan: VulkanGraphicsDevice: SetBufferData: buffer is nullptr!");
            return;
        }

        VulkanBuffer* vb = static_cast<VulkanBuffer*>(buffer);
        
        vb->SetData(m_Frames[m_FrameIndex].get(), m_FrameIndex, data, size);
    }

    void VulkanGraphicsDevice::OnDestroy()
    {
        m_Context->GetDevice().waitIdle();
        m_Frames.clear();
    }

} // namespace Engine
