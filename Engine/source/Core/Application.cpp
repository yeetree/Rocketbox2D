#include "Engine/Core/Application.h"
#include "Engine/Core/FileSystem.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"

#include "Engine/Platform/IGraphicsBridge.h"

//#include "Engine/Events/Event.h"
//#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/WindowEvent.h"
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
        m_Locator->Register<IPlatform>(m_Platform.get());
        
        // Set event callback
        //m_Platform->SetEventCallback(std::bind(&Engine::Application::EventCallback, this, std::placeholders::_1));
    }

    Application* Application::Get() { return s_Instance; }

    ServiceLocator* Application::GetServiceLocator() { return m_Locator.get(); }

    RHI::SwapChainHandle Application::GetSwapChain() { return m_SwapChain; }

    void Application::Init(const WindowProperties& properties) {
        
        // Initialize logger
        Log::Init();
        LOG_CORE_INFO("Engine version Rocketbox2D_In_Development");

        // Create window
        LOG_CORE_INFO("Creating window...");
        m_Window = std::move(m_Platform->CreateWindow(properties));
        m_Locator->Register<IWindow>(m_Window.get());

        // Init filesystem
        LOG_CORE_INFO("Initializing filesystem...");
        m_FileSystem = CreateScope<FileSystem>(m_Platform->GetBasePath());
        m_Locator->Register<FileSystem>(m_FileSystem.get());

        // Create graphics device
        LOG_CORE_INFO("Initializing graphics device...");
        m_GraphicsDevice = RHI::IGraphicsDevice::Create(m_Window->GetAPI());
        m_Locator->Register<RHI::IGraphicsDevice>(m_GraphicsDevice.get());

        // Create swapchain
        RHI::SwapChainDesc scdesc{
            .window = m_Window.get(),
            .presentation = RHI::PresentMode::VSync,
            .format = RHI::PixelFormat::RGBA8
        };
        m_SwapChain = m_GraphicsDevice->CreateSwapChain(scdesc);


        // Create Renderer2D
        //LOG_CORE_INFO("Initializing renderer...");
        //m_Renderer = CreateScope<Renderer>(m_GraphicsDevice.get());
        //s_Locator->Register<Renderer>(m_Renderer.get());

        // Create resource manager
        LOG_CORE_INFO("Initializing resource manager...");
        m_ResourceManager = CreateScope<ResourceManager>();
        m_Locator->Register<ResourceManager>(m_ResourceManager.get());

        // Create event manager
        LOG_CORE_INFO("Initializing event manager...");
        m_EventManager = CreateScope<EventManager>();
        m_Locator->Register<EventManager>(m_EventManager.get());

        // Create input
        LOG_CORE_INFO("Initializing input...");
        m_Input = CreateScope<Input>();
        m_Locator->Register<Input>(m_Input.get());

        // Subscribe to event callback
        // Bind both parameters (EventType and Event) so the resulting callable
        // matches the EventCallback signature expected by SubscribeAll.
        m_EventListenerID = m_EventManager->SubscribeAll(std::bind(&Engine::Application::EventCallback, this, std::placeholders::_1, std::placeholders::_2));

        LOG_CORE_INFO("Initialization complete.");
    }

    void Application::EventCallback(StringName type, const Event& event) {
        if(type == Hash32("Quit"))
        {
            m_Running = false;
        }
        if(type == Hash32("WindowResized"))
        {
            const WindowResizedEvent& wr = static_cast<const WindowResizedEvent&>(event);
            if(m_SwapChain.IsValid())
            {
                m_GraphicsDevice->ResizeSwapChain(m_SwapChain, wr.GetSizeX(), wr.GetSizeY());
            }
        }
        OnEvent(type, event);
    }

    void Application::Run() {
        if(m_Running)
            return;

        m_Running = true;
        m_Timer.Reset();

        StringName::Reset();

        OnStart();

        double timePrev = m_Timer.DGetTime();
        while(m_Running) {
            // Get time & dt
            float timeNow = m_Timer.GetTime();
            float dt = timeNow - timePrev;

            m_Platform->PollEvents(); // Process input events

            m_EventManager->FlushEvents();

            // Update
            OnUpdate(dt);

            m_Input->Update();

            m_EventManager->FlushEvents();

            // Render
            m_GraphicsDevice->BeginFrame();
            OnRender();
            m_GraphicsDevice->EndFrame();

            // Update time
            timePrev = timeNow;
        }
        LOG_CORE_INFO("Shutting down...");
        OnDestroy();
        m_EventManager->UnsubscribeAll(m_EventListenerID);
        m_GraphicsDevice->OnDestroy();
    }

    Application::~Application() {

    }
} // namespace Engine
