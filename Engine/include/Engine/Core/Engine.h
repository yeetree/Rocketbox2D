#ifndef ENGINE_CORE_ENGINE_H
#define ENGINE_CORE_ENGINE_H

#include "engine_export.h"

#include <SDL3/SDL.h>

#include <string>

#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Core/ResourceManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Math/Vector.h"

namespace Engine {
    // Main engine class, handles game loop.
    class ENGINE_EXPORT Engine {
    public:
        Engine();
        ~Engine();

        void Init(int width, int height, std::string title, SDL_WindowFlags flags);
        void Run();

        IGraphicsDevice& GetGraphicsDevice() { return *m_GraphicsDevice; }
        Renderer2D& GetRenderer2D() { return *m_Renderer2D; }
        ResourceManager& GetResourceManager() { return *m_ResourceManager; }
        Input& GetInput() { return *m_Input; }

        int GetWindowWidth() { return m_WindowWidth; }
        int GetWindowHeight() { return m_WindowHeight; }
        iVec2 GetWindowSize() { return iVec2(m_WindowWidth, m_WindowHeight); }

        virtual void OnStart() = 0;

        virtual void OnInput(SDL_Event event) = 0;
        virtual void OnUpdate(float dt) = 0;
        virtual void OnRender() = 0;

        virtual void OnDestroy() = 0;
        
    private:
        bool m_Running;

        SDL_Window* m_Window;
        int m_WindowWidth, m_WindowHeight;

        uint64_t m_TicksPrevious;
        
        std::unique_ptr<IGraphicsDevice> m_GraphicsDevice;
        std::unique_ptr<Renderer2D> m_Renderer2D;
        std::unique_ptr<ResourceManager> m_ResourceManager;
        std::unique_ptr<Input> m_Input;
    };
} // namespace Engine


#endif // CORE_ENGINE_H