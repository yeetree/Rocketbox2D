#ifndef RHI_VULKAN_VULKANGRAPHICSDEVICE
#define RHI_VULKAN_VULKANGRAPHICSDEVICE

#include "Engine/RHI/IGraphicsDevice.h"

#include "RHI/Vulkan/IVulkanGraphicsBridge.h"

namespace Engine
{
    class ENGINE_EXPORT VulkanGraphicsDevice : public IGraphicsDevice
    {
    private:
        Scope<IVulkanGraphicsBridge> m_Bridge;

    public:
        VulkanGraphicsDevice(Scope<IVulkanGraphicsBridge> bridge);
        ~VulkanGraphicsDevice() = default;

        Scope<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) override;

        // Frame pacing
        void BeginFrame() override;
        void EndFrame() override;

        // Commands
        ICommandBuffer* GetCommandBuffer() override;
        void Submit(ICommandBuffer* cmd) override;

        void OnDestroy() override;
    };
} // namespace Engine


#endif // RHI_VULKAN_VULKANGRAPHICSDEVICE
