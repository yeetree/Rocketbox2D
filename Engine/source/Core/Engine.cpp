#include "Engine/Core/Engine.h"
#include "Engine/Core/Filesystem.h"
#include <iostream>

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
        std::cout << "Engine indev" << std::endl;

        m_WindowWidth = width;
        m_WindowHeight = height;

        // Create window
        // Create window with OpenGL -- everything else is platform agnostic, so switching backends will be insanely trivial in the future
        m_Window = SDL_CreateWindow(title.c_str(), m_WindowWidth, m_WindowHeight, flags | SDL_WINDOW_OPENGL);
        if(m_Window == nullptr)
        {
            SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
            return; // Failure
        }

        // Create graphics device
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::OpenGL, m_Window);
        m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);

        // Create Renderer2D
        m_Renderer2D = std::make_unique<Renderer2D>(m_GraphicsDevice.get());

        // Init filesystem (set base path)
        FileSystem::SetBasePath(SDL_GetBasePath());

        // Create resource manager
        m_ResourceManager = std::make_unique<ResourceManager>(m_GraphicsDevice.get());
    }

    void Engine::Run() {
        if(m_Running)
            return;

        m_Running = true;

        Startup();

        m_TicksPrevious = SDL_GetTicks();
        while(m_Running) {
            // Get ticks & dt
            uint64_t ticksNow = SDL_GetTicks();
            float dt = (ticksNow - m_TicksPrevious) / 1000.0f;

            // Input
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_EVENT_QUIT:
                        m_Running = false;
                        break;
                    case SDL_EVENT_WINDOW_RESIZED:
                        SDL_GetWindowSize(m_Window, &m_WindowWidth, &m_WindowHeight);
                        m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);
                        break;
                }
                Input(event);
            }

            // Update
            Update(dt);

            // Render
            m_GraphicsDevice->BeginFrame();
            Render();
            m_GraphicsDevice->EndFrame();
            m_GraphicsDevice->Present();

            // Update ticks
            m_TicksPrevious = ticksNow;
        }
        Cleanup();
    }

    Engine::~Engine() {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }
} // namespace Engine
