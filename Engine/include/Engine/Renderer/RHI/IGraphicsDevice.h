#ifndef ENGINE_RENDERER_IGRAPHICSDEVICE
#define ENGINE_RENDERER_IGRAPHICSDEVICE

#include "engine_export.h"

#include <memory>
#include <cstdint>

#include <SDL3/SDL.h>

#include "Engine/Renderer/RHI/IBuffer.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/IPipelineState.h"

namespace Engine {
    // What backend to create (Only OpenGL for now)
    enum class GraphicsAPI { OpenGL };

    // Creates resources for rendering and dispatches draw calls
    class ENGINE_EXPORT IGraphicsDevice {
    public:
        // Static function to create a graphics device implementation and select backend
        static std::unique_ptr<IGraphicsDevice> Create(GraphicsAPI api, SDL_Window* window);

        virtual ~IGraphicsDevice() = default;

        // Resource creation
        virtual std::unique_ptr<IBuffer>  CreateBuffer(const BufferDesc& desc) = 0;
        virtual std::unique_ptr<ITexture> CreateTexture(const TextureDesc& desc) = 0;
        virtual std::unique_ptr<IShader>  CreateShader(const ShaderDesc& desc) = 0;
        
        // Pipeline creation
        virtual std::unique_ptr<IPipelineState> CreatePipelineState(const PipelineDesc& desc) = 0;

        // Render and swapchain
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        
        // Draw call
        virtual void SubmitDraw(uint32_t indexCount) = 0;

        // Get back buffer
        //virtual ITexture* GetBackBuffer() = 0;

        // Resize
        virtual void Resize(int width, int height) = 0;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_IGRAPHICSDEVICE
