#include "Engine/Core/Engine.h"

namespace Engine {

    Engine::Engine() : m_Running(false) {
        // Initialize SDL
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
            return; // Failure
        }
    }

    void Engine::Init(int width, int height, std::string title, SDL_WindowFlags flags) {
        // Create window
        // Create window with OpenGL -- everything else is platform agnostic, so switching backends will be insanely trivial in the future
        m_Window = SDL_CreateWindow(title.c_str(), width, height, flags | SDL_WINDOW_OPENGL);
        if(m_Window == nullptr)
        {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            return; // Failure
        }

        // Create graphics device
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::OpenGL, m_Window);
    }

    void Engine::Run() {
        if(m_Running)
            return;

        m_Running = true;

        Startup();
        while(m_Running) {
            // Input
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_EVENT_QUIT:
                        m_Running = false;
                        break;
                }
                Input(event);
            }

            // Update
            Update();

            // Render
            m_GraphicsDevice->BeginFrame();
            Render();
            m_GraphicsDevice->EndFrame();
            m_GraphicsDevice->Present();
        }
        Cleanup();
    }

    Engine::~Engine() {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }
} // namespace Engine
