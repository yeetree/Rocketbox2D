#ifndef ENGINE_RHI_IGRAPHICSDEVICE
#define ENGINE_RHI_IGRAPHICSDEVICE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/IGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ISwapchain.h"
#include "Engine/RHI/ITexture.h"

namespace Engine
{
    // IGraphicsDevice represents the GPU itself. It handles resource creation and lifetime and memory management.
    class ENGINE_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;

        virtual Scope<ISwapchain> CreateSwapchain(const SwapchainDesc& desc) = 0;

        virtual void OnDestroy() = 0;

        static Scope<IGraphicsDevice> Create(GraphicsAPI api, Ref<IGraphicsBridge> graphicsBridge, Ref<IWindow> window);
    };
} // namespace Engine


#endif // ENGINE_RHI_IGRAPHICSDEVICE
