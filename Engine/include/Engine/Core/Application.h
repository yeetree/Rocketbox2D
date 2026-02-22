#ifndef ENGINE_CORE_APPLICATION
#define ENGINE_CORE_APPLICATION

#include "engine_export.h"

#include <SDL3/SDL.h>

#include <string>
#include <cstdint>

#include "Engine/Core/Base.h"

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

        void Init(int width, int height, std::string title, SDL_WindowFlags flags);
        void Run();

        IGraphicsDevice& GetGraphicsDevice();
        Renderer& GetRenderer();
        ResourceManager& GetResourceManager();
        Input& GetInput();

        int GetWindowWidth();
        int GetWindowHeight();
        iVec2 GetWindowSize();
        float GetAspectRatio();

        virtual void OnStart() = 0;

        virtual void OnInput(SDL_Event event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        static Application *s_Instance;

        bool m_Running;

        SDL_Window* m_Window;
        int m_WindowWidth, m_WindowHeight;
        float m_AspectRatio;

        uint64_t m_TicksPrevious;
        
        Scope<Input> m_Input;
        Scope<ResourceManager> m_ResourceManager;
        Scope<Renderer> m_Renderer;
        Scope<IGraphicsDevice> m_GraphicsDevice;
    };
} // namespace Engine


#endif // ENGINE_CORE_APPLICATION