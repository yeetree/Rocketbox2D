#include "Renderer/OpenGL/OpenGLGraphicsDevice.h"
#include "Renderer/OpenGL/OpenGLPipelineState.h"
#include "Renderer/OpenGL/OpenGLBuffer.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include <glad/gl.h>

#include <iostream>

namespace Engine {
    
    OpenGLGraphicsDevice::OpenGLGraphicsDevice(SDL_Window* window) : m_Window(window) {
        // Set attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Create context
        m_Context = SDL_GL_CreateContext(m_Window);
        
         // Initialize glad
        int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

        if (version == 0) {
            SDL_Log("Failed to initialize GLAD");
            return;
        }

        // Print OpenGL version
        int major = GLAD_VERSION_MAJOR(version);
        int minor = GLAD_VERSION_MINOR(version);
        SDL_Log("Loaded OpenGL version %d.%d", major, minor);

        // Make current
        SDL_GL_MakeCurrent(m_Window, m_Context);
        SDL_GL_SetSwapInterval(1);
    }

    OpenGLGraphicsDevice::~OpenGLGraphicsDevice() {

    };

    // Resource Creation
    std::unique_ptr<IBuffer> OpenGLGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return std::make_unique<OpenGLBuffer>(desc);
    };

    std::unique_ptr<ITexture> OpenGLGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return nullptr;
    };

    std::unique_ptr<IShader> OpenGLGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return std::make_unique<OpenGLShader>(desc);
    };

    std::unique_ptr<IPipelineState> OpenGLGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return std::make_unique<OpenGLPipelineState>(desc);
    };

    // Frame Management
    void OpenGLGraphicsDevice::BeginFrame() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark Grey
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };

    void OpenGLGraphicsDevice::EndFrame() {
        // Empty in OpenGL
    };

    void OpenGLGraphicsDevice::Present() {
        SDL_GL_SwapWindow(m_Window);
    };

    // Draw call
    void OpenGLGraphicsDevice::SubmitDraw(uint32_t indexCount) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    };

} // namespace Engine
