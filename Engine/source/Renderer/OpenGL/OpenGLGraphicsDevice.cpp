#include "Renderer/OpenGL/OpenGLGraphicsDevice.h"
#include "Renderer/OpenGL/OpenGLPipelineState.h"
#include "Renderer/OpenGL/OpenGLBuffer.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLTexture.h"
#include "Engine/Core/Log.h"
#include <glad/gl.h>

namespace Engine {
    
    OpenGLGraphicsDevice::OpenGLGraphicsDevice(SDL_Window* window) : m_Window(window) {
        LOG_CORE_INFO("OpenGL: Creating OpenGL graphics device...");
        
        // Set attributes
        // Use OpenGL 3.3 Core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        // Create context
        LOG_CORE_INFO("OpenGL: Creating OpenGL context...");
        m_Context = SDL_GL_CreateContext(m_Window);
        if(m_Context == nullptr) {
            LOG_CORE_CRITICAL("OpenGL: OpenGL context could not be created! SDL: {0}", SDL_GetError());
            return; // Failure
        }
        
        // Initialize glad
        LOG_CORE_INFO("OpenGL: Initializing GLAD...");
        int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

        if (version == 0) {
            LOG_CORE_CRITICAL("OpenGL: Failed to initialize GLAD!");
            return; // Failure
        }

        // Print OpenGL version
        int major = GLAD_VERSION_MAJOR(version);
        int minor = GLAD_VERSION_MINOR(version);
        LOG_CORE_INFO("OpenGL: Loaded OpenGL version {0}.{1}", major, minor);

        // Make current
        SDL_GL_MakeCurrent(m_Window, m_Context);
        SDL_GL_SetSwapInterval(1);
    }

    OpenGLGraphicsDevice::~OpenGLGraphicsDevice() {

    };

    // Resource Creation
    std::unique_ptr<IBuffer> OpenGLGraphicsDevice::CreateBuffer(const BufferDesc& desc) {
        return std::make_unique<OpenGLBuffer>(desc);
    }

    std::unique_ptr<ITexture> OpenGLGraphicsDevice::CreateTexture(const TextureDesc& desc) {
        return std::make_unique<OpenGLTexture>(desc);
    }

    std::unique_ptr<IShader> OpenGLGraphicsDevice::CreateShader(const ShaderDesc& desc) {
        return std::make_unique<OpenGLShader>(desc);
    }

    std::unique_ptr<IPipelineState> OpenGLGraphicsDevice::CreatePipelineState(const PipelineDesc& desc) {
        return std::make_unique<OpenGLPipelineState>(desc);
    }

    // Frame Management
    void OpenGLGraphicsDevice::BeginFrame() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark Grey
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGraphicsDevice::EndFrame() {
        // Empty in OpenGL
    }

    void OpenGLGraphicsDevice::Present() {
        SDL_GL_SwapWindow(m_Window);
    }

    // Draw call
    void OpenGLGraphicsDevice::SubmitDraw(uint32_t indexCount) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }

    // Resize
    void OpenGLGraphicsDevice::Resize(int width, int height) {
        glViewport(0, 0, width, height);
    }

} // namespace Engine
