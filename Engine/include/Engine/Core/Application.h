#ifndef _ENGINE_CORE_APPLICATION
#define _ENGINE_CORE_APPLICATION

#include "engine_export.h"

#include <string>
#include <cstdint>

#include "Engine/Core/Base.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/ServiceLocator.h"
#include "Engine/Core/FileSystem.h"

#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IWindow.h"

#include "Engine/RHI/IGraphicsDevice.h"

#include "Engine/Resources/ResourceManager.h"

#include "Engine/Events/EventManager.h"

#include "Engine/Input/Input.h"

#include "Engine/Math/Vector.h"

namespace Engine {
    // Main application class, handles game loop.
    class ENGINE_EXPORT Application {
    public:
        Application();
        ~Application();

        static Application* Get();

        ServiceLocator* GetServiceLocator();
        RHI::SwapChainHandle GetSwapChain();

        void Init(const WindowProperties& properties);
        //void EventCallback(Event& event);
        void Run();

        virtual void OnStart() = 0;

        virtual void OnEvent(StringName type, const Event& event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        static Application *s_Instance;

        Scope<ServiceLocator> m_Locator;
        
        Scope<FileSystem> m_FileSystem;
        Scope<IPlatform> m_Platform;
        Scope<RHI::IGraphicsDevice> m_GraphicsDevice;
        Scope<IWindow> m_Window;
        RHI::SwapChainHandle m_SwapChain;
        Scope<ResourceManager> m_ResourceManager;
        Scope<EventManager> m_EventManager;
        Scope<Input> m_Input;
        //Scope<Renderer> m_Renderer;

        void EventCallback(StringName type, const Event& event);
        EventListenerID m_EventListenerID;

        bool m_Running;
        Timer m_Timer;
    };
} // namespace Engine


#endif // _ENGINE_CORE_APPLICATION
