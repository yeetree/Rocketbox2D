#ifndef ENGINE_CORE_APPLICATION
#define ENGINE_CORE_APPLICATION

#include "engine_export.h"

#include <string>
#include <cstdint>

#include "Engine/Core/Base.h"
#include "Engine/Core/Timer.h"
#include "Engine/Core/ServiceLocator.h"

#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IWindow.h"

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

        void Init(const WindowProperties& properties);
        //void EventCallback(Event& event);
        void Run();

        virtual void OnStart() = 0;

        //virtual void OnEvent(Event& event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        static Application *s_Instance;

        Scope<ServiceLocator> m_Locator;
        
        //Scope<Input> m_Input;
        //Scope<FileSystem> m_FileSystem;
        Ref<IPlatform> m_Platform;
        Ref<IWindow> m_Window;
        Ref<ResourceManager> m_ResourceManager;
        Ref<EventManager> m_EventManager;
        Ref<Input> m_Input;
        //Scope<Renderer> m_Renderer;

        void EventCallback(StringName type, const Event& event);
        EventListenerID m_EventListenerID;

        bool m_Running;
        Timer m_Timer;
    };
} // namespace Engine


#endif // ENGINE_CORE_APPLICATION
