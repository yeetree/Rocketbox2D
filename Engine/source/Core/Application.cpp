#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include <iostream>
#include <cstdint>

namespace Engine {

    Application *Application::s_Instance = nullptr;

    Application::Application() : m_Running(false) {
        if (s_Instance) {
            return; 
        }
        s_Instance = this;

        // Initialize SDL
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            LOG_CORE_ERROR("SDL could not initialize! SDL: {0}", SDL_GetError());
            return; // Failure
        }
    }

    Application& Application::Get() { return *s_Instance; }

    IGraphicsDevice& Application::GetGraphicsDevice() { return *m_GraphicsDevice; }
    Renderer2D& Application::GetRenderer2D() { return *m_Renderer2D; }
    ResourceManager& Application::GetResourceManager() { return *m_ResourceManager; }
    Input& Application::GetInput() { return *m_Input; }

    int Application::GetWindowWidth() { return m_WindowWidth; }
    int Application::GetWindowHeight() { return m_WindowHeight; }
    iVec2 Application::GetWindowSize() { return iVec2(m_WindowWidth, m_WindowHeight); }

    void Application::Init(int width, int height, std::string title, SDL_WindowFlags flags) {
        
        // Initialize logger
        Log::Init();
        LOG_CORE_INFO("Engine indev");

        m_WindowWidth = width;
        m_WindowHeight = height;

        // Create window
        // Create window with OpenGL -- everything else is platform agnostic, so switching backends will be insanely trivial in the future
        LOG_CORE_INFO("Creating window...");
        m_Window = SDL_CreateWindow(title.c_str(), m_WindowWidth, m_WindowHeight, flags | SDL_WINDOW_OPENGL);
        if(m_Window == nullptr)
        {
            LOG_CORE_CRITICAL("Window could not be created! SDL: {0}", SDL_GetError());
            return; // Failure
        }

        // Create input
        LOG_CORE_INFO("Initializing input...");
        m_Input = std::make_unique<Input>();
        Input::s_Instance = m_Input.get(); // Engine is friend to Input class, we set it's instance for it.

        // Create graphics device
        LOG_CORE_INFO("Initializing graphics device...");
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::OpenGL, m_Window);
        m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);

        // Create Renderer2D
        LOG_CORE_INFO("Initializing renderer...");
        m_Renderer2D = std::make_unique<Renderer2D>(m_GraphicsDevice.get());

        // Init filesystem (set base path)
        LOG_CORE_INFO("Initializing filesystem...");
        FileSystem::SetBasePath(SDL_GetBasePath());

        // Create resource manager
        LOG_CORE_INFO("Initializing resource manager...");
        m_ResourceManager = std::make_unique<ResourceManager>(m_GraphicsDevice.get());

        LOG_CORE_INFO("Initialization complete.");
    }

    void Application::Run() {
        if(m_Running)
            return;

        m_Running = true;

        OnStart();

        m_TicksPrevious = SDL_GetTicks();
        while(m_Running) {
            // Get ticks & dt
            uint64_t ticksNow = SDL_GetTicks();
            float dt = (ticksNow - m_TicksPrevious) / 1000.0f;

            // Input
            m_Input->OnUpdate(); // Process input in Input first

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
            OnUpdate(dt);

            // Render
            m_GraphicsDevice->BeginFrame();
            OnRender();
            m_GraphicsDevice->EndFrame();
            m_GraphicsDevice->Present();

            // Update ticks
            m_TicksPrevious = ticksNow;
        }
        LOG_CORE_INFO("Shutting down...");
        OnDestroy();
    }

    Application::~Application() {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }
} // namespace Engine
