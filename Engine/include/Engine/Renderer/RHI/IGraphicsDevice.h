#ifndef ENGINE_RENDERER_RHI_IGRAPHICSDEVICE
#define ENGINE_RENDERER_RHI_IGRAPHICSDEVICE

#include "engine_export.h"

#include <cstdint>


#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/IPipelineState.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"
#include "Engine/Platform/IWindow.h"
#include "Engine/Platform/IGraphicsBridge.h"

namespace Engine {
    // Creates resources for rendering and dispatches draw calls
    class ENGINE_EXPORT IGraphicsDevice {
    public:
        // Static function to create a graphics device implementation and select backend
        static Scope<IGraphicsDevice> Create(GraphicsAPI api, IGraphicsBridge* graphicsBridge, IWindow* window);

        virtual ~IGraphicsDevice() = default;

        // Resource creation
        virtual Scope<IBuffer>          CreateBuffer(const BufferDesc& desc) = 0;
        virtual Scope<ITexture>         CreateTexture(const TextureDesc& desc) = 0;
        virtual Scope<IShader>          CreateShader(const ShaderDesc& desc) = 0;
        
        // Pipeline creation
        virtual Scope<IPipelineState> CreatePipelineState(const PipelineDesc& desc) = 0;

        // Render and swapchain
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void SetClearColor(Vec4 color) = 0;
        virtual void SetVSync(bool vsync) = 0;
        virtual bool IsVSync() = 0;
        
        virtual void BindPipelineState(IPipelineState& pipeline) = 0;
        virtual void BindVertexBuffer(IBuffer& buffer) = 0;
        virtual void BindIndexBuffer(IBuffer& buffer) = 0;
        virtual void BindUniformBuffer(IBuffer& buffer, uint32_t slot) = 0;
        virtual void BindTexture(ITexture& texture, uint32_t slot) = 0;
        virtual void PushConstants(const void* data, uint32_t size) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;

        // Resize
        virtual void UpdateSwapchain() = 0;
    private:
        friend class Application;
        // Gives GraphicsDevice chance to finish current work before app can destroy
        virtual void OnDestroy() = 0;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_RHI_IGRAPHICSDEVICE
