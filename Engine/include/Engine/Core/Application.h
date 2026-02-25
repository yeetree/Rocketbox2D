#ifndef ENGINE_CORE_APPLICATION
#define ENGINE_CORE_APPLICATION

#include "engine_export.h"

#include <string>
#include <cstdint>

#include "Engine/Core/Base.h"

#include "Engine/Events/Event.h"

#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IWindow.h"
#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Core/ResourceManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Math/Vector.h"

namespace Engine {
    // Main application class, handles game loop.
    class ENGINE_EXPORT Application {
    public:
        Application();
        ~Application();

        static Application& Get();

        void Init(const WindowProperties& properties);
        void EventCallback(Event& event);
        void Run();

        IGraphicsDevice& GetGraphicsDevice();
        Renderer& GetRenderer();
        ResourceManager& GetResourceManager();
        Input& GetInput();
        IWindow& GetWindow();

        int GetWindowWidth();
        int GetWindowHeight();
        iVec2 GetWindowSize();
        float GetAspectRatio();

        virtual void OnStart() = 0;

        virtual void OnEvent(Event& event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        static Application *s_Instance;

        bool m_Running;
        
        Scope<IPlatform> m_Platform;
        Scope<IWindow> m_Window;
        Scope<Input> m_Input;
        Scope<ResourceManager> m_ResourceManager;
        Scope<Renderer> m_Renderer;
        Scope<IGraphicsDevice> m_GraphicsDevice;
    };
} // namespace Engine


#endif // ENGINE_CORE_APPLICATION
