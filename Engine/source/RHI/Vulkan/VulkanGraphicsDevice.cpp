#include "RHI/Vulkan/VulkanGraphicsDevice.h"

#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

namespace Engine
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(Ref<IGraphicsBridge> bridge, Ref<IWindow> window)
    {
        LOG_CORE_INFO("Vulkan: Creating Vulkan graphics device...");

        ENGINE_CORE_ASSERT(bridge != nullptr, "Vulkan: VulkanGraphicsDevice: bridge is nullptr!");
        ENGINE_CORE_ASSERT(bridge->GetAPI() == GraphicsAPI::Vulkan, "Vulkan: VulkanGraphicsDevice: bridge is not for Vulkan!");

        m_GraphicsBridge = std::static_pointer_cast<IVulkanGraphicsBridge>(bridge);

        m_Context = CreateRef<VulkanContext>(m_GraphicsBridge, window);
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {

    }

    Scope<ISwapChain> VulkanGraphicsDevice::CreateSwapChain(const SwapChainDesc& desc)
    {
        return nullptr;
    }

} // namespace Engine
