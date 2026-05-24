#include "RHI/Vulkan/RHI/VulkanGraphicsDevice.h"

namespace Engine
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge)
        : m_Bridge(std::move(bridge))
    {
        
    };

    Scope<ISwapChain> VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        return nullptr;
    }

    // Frame pacing
    void VulkanGraphicsDevice::BeginFrame()
    {

    }

    void VulkanGraphicsDevice::EndFrame()
    {

    }

    // Commands
    ICommandBuffer* VulkanGraphicsDevice::GetCommandBuffer()
    {
        return nullptr;
    }

    void VulkanGraphicsDevice::Submit(ICommandBuffer* cmd)
    {

    }

    void VulkanGraphicsDevice::OnDestroy()
    {

    }

} // namespace Engine
