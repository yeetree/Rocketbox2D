#ifndef ENGINE_CORE_APPLICATION
#define ENGINE_CORE_APPLICATION

#include "engine_export.h"

#include <string>
#include <cstdint>

#include "Engine/Core/Base.h"

#include "Engine/Core/ServiceLocator.h"

#include "Engine/Resources/ResourceManager.h"
#include "Engine/Math/Vector.h"

namespace Engine {
    // Main application class, handles game loop.
    class ENGINE_EXPORT Application {
    public:
        Application();
        ~Application();

        static Application& Get();

        ServiceLocator& GetServiceLocator();

        void Init();
        //void EventCallback(Event& event);
        void Run();

        virtual void OnStart() = 0;

        //virtual void OnEvent(Event& event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        static Application *s_Instance;

        ServiceLocator *m_Locator;
        
        //Scope<Input> m_Input;
        //Scope<FileSystem> m_FileSystem;
        Scope<ResourceManager> m_ResourceManager;
        //Scope<Renderer> m_Renderer;

        bool m_Running;
    };
} // namespace Engine


#endif // ENGINE_CORE_APPLICATION
