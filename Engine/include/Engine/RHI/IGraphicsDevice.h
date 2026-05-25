#ifndef ENGINE_RHI_IGRAPHICSDEVICE
#define ENGINE_RHI_IGRAPHICSDEVICE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/IGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ISwapChain.h"
#include "Engine/RHI/ITexture.h"
#include "Engine/RHI/ICommandBuffer.h"

namespace Engine
{
    // IGraphicsDevice represents the GPU itself. It handles resource creation and lifetime and memory management.
    class ENGINE_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;

        virtual Scope<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) = 0;

        // Frame pacing
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        // Swapchain passees
        virtual ICommandBuffer* BeginSwapChainPass(Ref<ISwapChain> swapchain) = 0;
        virtual void EndSwapChainPass(Ref<ISwapChain> swapchain, ICommandBuffer* cmd) = 0;

        virtual void OnDestroy() = 0;

        static Scope<IGraphicsDevice> Create(GraphicsAPI api);
    };
} // namespace Engine


#endif // ENGINE_RHI_IGRAPHICSDEVICE
