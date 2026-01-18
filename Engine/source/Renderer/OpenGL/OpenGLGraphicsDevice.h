#ifndef RENDERER_OPENGL_OPENGLGRAPHICSDEVICE
#define RENDERER_OPENGL_OPENGLGRAPHICSDEVICE

#include <SDL3/SDL.h>
#include <glad/gl.h>

#include "Engine/Renderer/RHI/IGraphicsDevice.h"

namespace Engine {
    // OpenGL Implementation of IPipelineState
    class OpenGLGraphicsDevice : public IGraphicsDevice {
    public:
        // Constructor: Creates an OpenGLGraphicsDevice with SDL_Window
        OpenGLGraphicsDevice(SDL_Window* window);
        virtual ~OpenGLGraphicsDevice();

        // Prevent copying
        OpenGLGraphicsDevice(const OpenGLGraphicsDevice&) = delete;
        OpenGLGraphicsDevice& operator=(const OpenGLGraphicsDevice&) = delete;

        // Resource Creation
        std::unique_ptr<IBuffer> CreateBuffer(const BufferDesc& desc) override;
        std::unique_ptr<ITexture> CreateTexture(const TextureDesc& desc) override;
        std::unique_ptr<IShader> CreateShader(const ShaderDesc& desc) override;
        std::unique_ptr<IPipelineState> CreatePipelineState(const PipelineDesc& desc) override;
        std::unique_ptr<IVertexArray> CreateVertexArray(const VertexArrayDesc& desc) override;

        // Frame Management
        void BeginFrame() override;
        void EndFrame() override;
        void Present() override;

        // Draw call
        void SubmitDraw(uint32_t indexCount) override;

        // Utility
        //ITexture* GetBackBuffer() override { return m_BackBuffer.get(); }

        // Resize
        void Resize(int width, int height) override;

    private:
        SDL_Window* m_Window;
        SDL_GLContext m_Context;

        std::unique_ptr<ITexture> m_BackBuffer;
    };

} // namespace Engine

#endif // RENDERER_OPENGL_OPENGLGRAPHICSDEVICE
