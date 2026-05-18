#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

//#include "Engine/Events/Event.h"
//#include "Engine/Events/ApplicationEvent.h"
//#include "Engine/Events/WindowEvent.h"
//#include "Engine/Events/KeyEvent.h"
//#include "Engine/Events/MouseEvent.h"

#include <iostream>
#include <cstdint>


namespace Engine {

    Application *Application::s_Instance = nullptr;

    Application::Application() : m_Running(false), m_Timer() {
        if (s_Instance) {
            return; 
        }
        s_Instance = this;

        m_Locator = CreateScope<ServiceLocator>();

        // Initialize platform
        m_Platform = IPlatform::Create();
        ENGINE_CORE_ASSERT(m_Platform, "Platform is null!");
        
        // Set event callback
        //m_Platform->SetEventCallback(std::bind(&Engine::Application::EventCallback, this, std::placeholders::_1));
    }

    Application& Application::Get() { return *s_Instance; }

    ServiceLocator& Application::GetServiceLocator() { return *m_Locator; }

    void Application::Init(const WindowProperties& properties) {
        
        // Initialize logger
        Log::Init();
        LOG_CORE_INFO("Engine version Rocketbox2D_In_Development");

        // Create window
        LOG_CORE_INFO("Creating window...");
        m_Window = m_Platform->CreateWindow(properties);
        m_Locator->RegisterInstance<IWindow>(m_Window.get());
        

        // Create input
        //LOG_CORE_INFO("Initializing input...");
        //m_Input = CreateScope<Input>();
        //s_Locator->RegisterInstance<Input>(m_Input.get());

        // Init filesystem
        //LOG_CORE_INFO("Initializing filesystem...");
        //m_FileSystem = CreateScope<FileSystem>(m_Platform->GetBasePath());
        //s_Locator->RegisterInstance<FileSystem>(m_FileSystem.get());

        // Create graphics device
        //LOG_CORE_INFO("Initializing graphics device...");
        //m_GraphicsDevice = IGraphicsDevice::Create(GraphicsAPI::Vulkan, &m_Platform->GetGraphicsBridge(), m_Window.get());
        //s_Locator->RegisterInstance<IGraphicsDevice>(m_GraphicsDevice.get());

        // Create Renderer2D
        //LOG_CORE_INFO("Initializing renderer...");
        //m_Renderer = CreateScope<Renderer>(m_GraphicsDevice.get());
        //s_Locator->RegisterInstance<Renderer>(m_Renderer.get());

        // Create resource manager
        LOG_CORE_INFO("Initializing resource manager...");
        m_ResourceManager = CreateScope<ResourceManager>();
        m_Locator->RegisterInstance<ResourceManager>(m_ResourceManager.get());

        LOG_CORE_INFO("Initialization complete.");
    }

    //void Application::EventCallback(Event& event) {
        /*
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
        OnEvent(event);*/
    //}

    void Application::Run() {
        if(m_Running)
            return;

        m_Running = true;
        m_Timer.Reset();

        OnStart();

        double timePrev = m_Timer.DGetTime();
        while(m_Running) {
            // Get time & dt
            //double timeNow = m_Platform->GetTime();
            //float dt = static_cast<float>(timeNow - timePrev);
            float dt = 0;

            // Input
            //m_Input->OnUpdate(); // Process input in Input first

            //m_Platform->PollEvents(); // Process input events

            // Update
            OnUpdate(dt);

            // Render
            //m_GraphicsDevice->BeginFrame();
            OnRender();
            //m_GraphicsDevice->EndFrame();
            //m_GraphicsDevice->Present();

            // Update time
            //timePrev = timeNow;
        }
        LOG_CORE_INFO("Shutting down...");
        //m_GraphicsDevice->OnDestroy();
        OnDestroy();
    }

    Application::~Application() {
        m_ResourceManager.reset();
        //m_Renderer.reset();
        //m_GraphicsDevice.reset();
    }
} // namespace Engine
