#ifndef ENGINE_RHI_ISWAPCHAIN
#define ENGINE_RHI_ISWAPCHAIN

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ITexture.h"
#include "Engine/RHI/ICommandBuffer.h"

namespace Engine
{
    enum class PresentMode
    {
        Immediate, // No vsync
        VSync, // Double buffered vsync
        Mailbox // Triple buffered vsync
    };

    // Decribes how a surface should be created
    struct SwapchainDesc {
        uint32_t width, height;
        PresentMode presentation;
        TextureFormat format;
    };

    // ISwapchain manages a collection of framebuffers that are rendering targets for the GPU
    // before being presented to the screen.
    class ENGINE_EXPORT ISwapchain {
    public:
        virtual ~ISwapchain() = default;

        // Swapchain config
        virtual void Resize(uint32_t width, uint32_t height) = 0; // Called on window resize events
        virtual void SetPresentation(PresentMode presentation) = 0;

        // Blocks until GPU is ready to begin drawing.
        // This CAN return nullptr if the GPU needs to skip this frame,
        // so make sure to check before recording anything!
        virtual ICommandBuffer* BeginFrame() = 0;

        // Presents image, begins working on next.
        virtual void EndFrame(ICommandBuffer* cmd) = 0;
    };
} // namespace Engine


#endif // ENGINE_RHI_ISWAPCHAIN
