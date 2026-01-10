#ifndef ENGINE_CORE_ENGINE_H
#define ENGINE_CORE_ENGINE_H

#include "engine_export.h"

#include <SDL3/SDL.h>

#include <string>

#include "Engine/Renderer/IGraphicsDevice.h"

namespace Engine {
    class ENGINE_EXPORT Engine {
    public:
        Engine();
        ~Engine();

        void Init(int width, int height, std::string title, SDL_WindowFlags flags);
        void Run();

        IGraphicsDevice& GetGraphicsDevice() { return *m_GraphicsDevice; }

        virtual void Startup() = 0;

        virtual void Input(SDL_Event event) = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;

        virtual void Cleanup() = 0;
        
    private:
        bool m_Running;
        SDL_Window* m_Window;
        std::unique_ptr<IGraphicsDevice> m_GraphicsDevice;
    };
} // namespace Engine


#endif // CORE_ENGINE_H