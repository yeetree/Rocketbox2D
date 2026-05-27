#ifndef ENGINE_RHI_IGRAPHICSDEVICE
#define ENGINE_RHI_IGRAPHICSDEVICE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/IGraphicsBridge.h"
#include "Engine/Platform/IWindow.h"

#include "Engine/RHI/GraphicsAPI.h"
#include "Engine/RHI/ISwapChain.h"
#include "Engine/RHI/ITexture.h"
#include "Engine/RHI/IShader.h"
#include "Engine/RHI/IPipeline.h"
#include "Engine/RHI/IBuffer.h"
#include "Engine/RHI/ICommandBuffer.h"

namespace Engine
{
    // IGraphicsDevice represents the GPU itself. It handles resource creation and lifetime, memory management,
    // and frame pacing.
    class ENGINE_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;

        virtual Scope<ISwapChain> CreateSwapChain(const SwapChainDesc& desc) = 0;
        virtual Scope<IShader> CreateShader(const ShaderDesc& desc) = 0;
        virtual Scope<IPipeline> CreatePipeline(const PipelineDesc& desc) = 0;
        virtual Scope<IBuffer> CreateBuffer(const BufferDesc& desc) = 0;

        // Frame pacing
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        // Single time commands
        virtual ICommandBuffer* BeginSingleTimeCommands() = 0;
        virtual void EndSingleTimeCommands(ICommandBuffer* cmd) = 0; // Blocks until completion

        // Swapchain passees
        virtual ICommandBuffer* BeginSwapChainPass(ISwapChain* swapchain) = 0;
        virtual void EndSwapChainPass(ISwapChain* swapchain, ICommandBuffer* cmd) = 0;

        // Swapchain config
        virtual void ResizeSwapChain(ISwapChain* swapchain, uint32_t width, uint32_t height) = 0; // Called on window resize events
        virtual void SetSwapChainPresentation(ISwapChain* swapchain, PresentMode presentation) = 0;

        // Dynamic buffers
        virtual void SetBufferData(IBuffer* buffer, void* data, size_t size) = 0;

        // Destroy
        virtual void OnDestroy() = 0;

        static Scope<IGraphicsDevice> Create(GraphicsAPI api);
    };
} // namespace Engine


#endif // ENGINE_RHI_IGRAPHICSDEVICE
