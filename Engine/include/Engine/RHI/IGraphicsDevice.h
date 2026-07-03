#ifndef ENGINE_RHI_IGRAPHICSDEVICE
#define ENGINE_RHI_IGRAPHICSDEVICE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/RHI/RHI.h"

#include "Engine/Math/Vector.h"

namespace Engine::RHI
{
    // Forward declaration
    class ICommandBuffer;

    // IGraphicsDevice represents the GPU itself. It handles resource creation and lifetime, memory management,
    // and frame pacing.
    class ENGINE_EXPORT IGraphicsDevice
    {
    public:
        virtual ~IGraphicsDevice() = default;

        // Resource creation
        virtual BufferHandle    CreateBuffer(const BufferDesc& desc) = 0;
        virtual TextureHandle   CreateTexture(const TextureDesc& desc) = 0;
        virtual ShaderHandle    CreateShader(const ShaderDesc& desc) = 0;
        virtual PipelineHandle  CreatePipeline(const PipelineDesc& desc) = 0;
        virtual SwapChainHandle CreateSwapChain(const SwapChainDesc& desc) = 0;

        // Resource destruction
        virtual void DestroyBuffer(BufferHandle& buffer) = 0;
        virtual void DestroyTexture(TextureHandle& texture) = 0;
        virtual void DestroyShader(ShaderHandle& shader) = 0;
        virtual void DestroyPipeline(PipelineHandle& pipeline) = 0;
        virtual void DestroySwapChain(SwapChainHandle& swapchain) = 0;

        // TODO: IGraphicsDevice: EnqueueUploadBuffer, EnqueueUploadTexture, and FlushUploads
        // Enqueues uploads into an immediate command buffer.

        // Frame pacing
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        // Render passes
        // virtual ICommandBuffer* BeginPass(TextureHandle renderTarget, Vec4 clearColor) = 0;
        virtual ICommandBuffer* BeginPass(SwapChainHandle renderTarget, Vec4 clearColor, TextureHandle depthBuffer = {}) = 0;
        virtual void EndPass(ICommandBuffer* cmd) = 0;

        // Immediate command buffer
        virtual ICommandBuffer* BeginImmediate() = 0;
        virtual void EndImmediate(ICommandBuffer* cmd) = 0; // Blocks until GPU is finished with work

        // Swapchain configuration
        virtual void ResizeSwapChain(SwapChainHandle swapchain, uint32_t width, uint32_t height) = 0;  // Called on window resize events
        virtual void SetSwapChainPresentMode(SwapChainHandle swapchain, PresentMode mode) = 0;

        // Destroy
        virtual void OnDestroy() = 0; // Called when application attempts to exit gracefully

        // Static factory creation
        static Scope<IGraphicsDevice> Create(GraphicsAPI api);
    };
} // namespace Engine::RHI


#endif // ENGINE_RHI_IGRAPHICSDEVICE
