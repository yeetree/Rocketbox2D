#ifndef ENGINE_RHI_ISWAPCHAIN
#define ENGINE_RHI_ISWAPCHAIN

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/IWindow.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ITexture.h"
#include "Engine/RHI/ICommandBuffer.h"

namespace Engine
{
    enum class ENGINE_EXPORT PresentMode
    {
        Immediate, // No vsync
        VSync, // Double buffered vsync
        Mailbox // Triple buffered vsync
    };

    // Decribes how a surface should be created
    struct ENGINE_EXPORT SwapChainDesc {
        IWindow* window;
        PresentMode presentation;
        TextureFormat format;
    };

    // ISwapChain manages a collection of framebuffers that are rendering targets for the GPU
    // before being presented to the screen.
    class ENGINE_EXPORT ISwapChain {
    public:
        virtual ~ISwapChain() = default;

        virtual ITexture* GetCurrentBackBuffer() = 0;
    };
} // namespace Engine


#endif // ENGINE_RHI_ISWAPCHAIN
