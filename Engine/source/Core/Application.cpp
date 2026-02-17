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
    Renderer& Application::GetRenderer() { return *m_Renderer; }
    ResourceManager& Application::GetResourceManager() { return *m_ResourceManager; }
    Input& Application::GetInput() { return *m_Input; }

    int Application::GetWindowWidth() { return m_WindowWidth; }
    int Application::GetWindowHeight() { return m_WindowHeight; }
    iVec2 Application::GetWindowSize() { return iVec2(m_WindowWidth, m_WindowHeight); }
    float Application::GetAspectRatio() { return m_AspectRatio; }

    void Application::Init(int width, int height, std::string title, SDL_WindowFlags flags) {
        
        // Initialize logger
        Log::Init();
        LOG_CORE_INFO("Engine version Rocketbox2D_In_Development");

        m_WindowWidth = width;
        m_WindowHeight = height;
        m_AspectRatio = (float)m_WindowWidth / (float)m_WindowHeight;

        // Create window
        // Create window with Vulkan -- everything else is platform agnostic, so switching backends will be insanely trivial in the future
        LOG_CORE_INFO("Creating window...");
        m_Window = SDL_CreateWindow(title.c_str(), m_WindowWidth, m_WindowHeight, flags | SDL_WINDOW_VULKAN);
        if(m_Window == nullptr)
        {
            LOG_CORE_CRITICAL("Window could not be created! SDL: {0}", SDL_GetError());
            return; // Failure
        }

        // Create input
        LOG_CORE_INFO("Initializing input...");
        m_Input = CreateScope<Input>();
        Input::s_Instance = m_Input.get(); // Engine is friend to Input class, we set it's instance for it.

        // Init filesystem (set base path)
        LOG_CORE_INFO("Initializing filesystem...");
        FileSystem::SetBasePath(SDL_GetBasePath());

        // Create graphics device
        LOG_CORE_INFO("Initializing graphics device...");
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::Vulkan, m_Window);
        m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);

        // Create Renderer2D
        LOG_CORE_INFO("Initializing renderer...");
        m_Renderer = CreateScope<Renderer>(m_GraphicsDevice.get());

        // Create resource manager
        LOG_CORE_INFO("Initializing resource manager...");
        m_ResourceManager = CreateScope<ResourceManager>(m_GraphicsDevice.get());

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
                        m_WindowWidth = event.window.data1;
                        m_WindowHeight = event.window.data2;
                        m_AspectRatio = (float)m_WindowWidth / (float)m_WindowHeight;
                        m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);
                        break;
                }
                OnInput(event);
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
        m_GraphicsDevice->OnDestroy();
        OnDestroy();
    }

    Application::~Application() {
        if(m_Window)
            SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }
} // namespace Engine
