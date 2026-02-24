#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include <iostream>
#include <cstdint>

#include <SDL3/SDL.h>

namespace Engine {

    Application *Application::s_Instance = nullptr;

    Application::Application() : m_Running(false) {
        if (s_Instance) {
            return; 
        }
        s_Instance = this;

        // Initialize platform
        m_Platform = IPlatform::Create(GraphicsAPI::Vulkan);
        ENGINE_CORE_ASSERT(m_Platform, "Platform is null!");
    }

    Application& Application::Get() { return *s_Instance; }

    IGraphicsDevice& Application::GetGraphicsDevice() { return *m_GraphicsDevice; }
    Renderer& Application::GetRenderer() { return *m_Renderer; }
    ResourceManager& Application::GetResourceManager() { return *m_ResourceManager; }
    Input& Application::GetInput() { return *m_Input; }

    int Application::GetWindowWidth() { return m_Window->GetWidth(); }
    int Application::GetWindowHeight() { return m_Window->GetHeight(); }
    iVec2 Application::GetWindowSize() { return iVec2(m_Window->GetWidth(), m_Window->GetHeight()); }
    float Application::GetAspectRatio() {
        if (!m_Window) {
            return 16.0f / 9.0f; 
        }
        return (float)(m_Window->GetWidth()) / (float)(m_Window->GetHeight());
    }

    void Application::Init(const WindowProperties& properties) {
        
        // Initialize logger
        Log::Init();
        LOG_CORE_INFO("Engine version Rocketbox2D_In_Development");

        // Create window
        LOG_CORE_INFO("Creating window...");
        m_Window = m_Platform->CreateWindow(properties);

        // Create input
        LOG_CORE_INFO("Initializing input...");
        //m_Input = CreateScope<Input>();
        //Input::s_Instance = m_Input.get(); // Engine is friend to Input class, we set it's instance for it.

        // Init filesystem (set base path)
        LOG_CORE_INFO("Initializing filesystem...");
        FileSystem::SetBasePath(SDL_GetBasePath());

        // Create graphics device
        LOG_CORE_INFO("Initializing graphics device...");
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::Vulkan, &m_Platform->GetGraphicsBridge(), m_Window.get());
        //m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);

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
            //m_Input->OnUpdate(); // Process input in Input first

            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_EVENT_QUIT:
                        m_Running = false;
                        break;
                    case SDL_EVENT_WINDOW_RESIZED:
                        //m_GraphicsDevice->Resize(m_WindowWidth, m_WindowHeight);
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
        m_ResourceManager.reset();
        m_Renderer.reset();
        m_GraphicsDevice.reset();
    }
} // namespace Engine
