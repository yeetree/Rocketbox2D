#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/WindowEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include <iostream>
#include <cstdint>


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

        // Set event callback
        m_Platform->SetEventCallback(std::bind(&Engine::Application::EventCallback, this, std::placeholders::_1));
    }

    Application& Application::Get() { return *s_Instance; }

    IGraphicsDevice& Application::GetGraphicsDevice() { return *m_GraphicsDevice; }
    Renderer& Application::GetRenderer() { return *m_Renderer; }
    ResourceManager& Application::GetResourceManager() { return *m_ResourceManager; }
    //Input& Application::GetInput() { return *m_Input; }

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
        m_Input = CreateScope<Input>();
        Input::s_Instance = m_Input.get(); // Engine is friend to Input class, we set it's instance for it.

        // Init filesystem (set base path)
        LOG_CORE_INFO("Initializing filesystem...");
        FileSystem::SetBasePath(m_Platform->GetBasePath());

        // Create graphics device
        LOG_CORE_INFO("Initializing graphics device...");
        m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::Vulkan, &m_Platform->GetGraphicsBridge(), m_Window.get());

        // Create Renderer2D
        LOG_CORE_INFO("Initializing renderer...");
        m_Renderer = CreateScope<Renderer>(m_GraphicsDevice.get());

        // Create resource manager
        LOG_CORE_INFO("Initializing resource manager...");
        m_ResourceManager = CreateScope<ResourceManager>(m_GraphicsDevice.get());

        LOG_CORE_INFO("Initialization complete.");
    }

    void Application::EventCallback(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<QuitEvent>([this](QuitEvent& e){
            m_Running = false;
            return false;
        });

        dispatcher.Dispatch<WindowClosedEvent>([this](WindowClosedEvent& e){
            // Check if it's closing the main window
            if(&e.GetWindow() == m_Window.get()) {
                m_Running = false;
                return true; // Only handle if it is the main window
            }
            return false;
        });

        dispatcher.Dispatch<WindowResizedEvent>([this](WindowResizedEvent& e){
            // Check if it's resizing the main window
            if(&e.GetWindow() == m_Window.get()) {
                m_GraphicsDevice->UpdateSwapchain(); // Ask GraphicsDevice to update swapchain
            }
            return false;
        });

        // pass event on
        m_Input->OnEvent(event);
        OnEvent(event);
    }

    void Application::Run() {
        if(m_Running)
            return;

        m_Running = true;

        OnStart();

        double timePrev = m_Platform->GetTime();
        while(m_Running) {
            // Get time & dt
            double timeNow = m_Platform->GetTime();
            float dt = static_cast<float>(timeNow - timePrev);

            // Input
            //m_Input->OnUpdate(); // Process input in Input first

            m_Platform->PollEvents(); // Process input events

            // Update
            OnUpdate(dt);

            // Render
            m_GraphicsDevice->BeginFrame();
            OnRender();
            m_GraphicsDevice->EndFrame();
            m_GraphicsDevice->Present();

            // Update time
            timePrev = timeNow;
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
