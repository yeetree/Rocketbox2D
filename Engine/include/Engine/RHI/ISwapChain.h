#ifndef ENGINE_RHI_ISWAPCHAIN
#define ENGINE_RHI_ISWAPCHAIN

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ITexture.h"

namespace Engine
{
    enum class PresentMode
    {
        Immediate, // No vsync
        VSync, // Double buffered vsync
        Mailbox // Triple buffered vsync
    };

    // Decribes how a surface should be created
    struct SwapChainDesc {
        uint32_t width, height;
        PresentMode presentation;
        TextureFormat format;
    };

    // ISwapChain manages a collection of framebuffers that are rendering targets for the GPU
    // before being presented to the screen.
    class ENGINE_EXPORT ISwapChain {
    public:
        virtual ~ISwapChain() = default;

        // SwapChain config
        virtual void Resize(uint32_t width, uint32_t height) = 0; // Called on window resize events
        virtual void SetPresentation(PresentMode presentation) = 0;

        // Blocks until GPU is ready to begin drawing
        virtual void BeginFrame() = 0;

        // Presents image, begins working on next.
        virtual void EndFrame() = 0;
    };
} // namespace Engine


#endif // ENGINE_RHI_ISWAPCHAIN
