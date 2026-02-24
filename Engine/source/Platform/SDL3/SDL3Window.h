#ifndef PLATFORM_SDL3_SDL3WINDOW
#define PLATFORM_SDL3_SDL3WINDOW

#include "Engine/Platform/IWindow.h"
#include <SDL3/SDL.h>

namespace Engine
{
    class SDL3Window : public IWindow {
    public:
        SDL3Window(const WindowProperties& properties);
        ~SDL3Window() override;

        void OnUpdate() override;

        unsigned int GetWidth() override;
        unsigned int GetHeight() override;

        void SetVSync(bool vsync) override;
        bool IsVSync() override;

        // Getters for SDL3*GraphicsBridge classes (not declared in IWindow)
        SDL_Window* GetSDLWindow();

    private:
        SDL_Window* m_Window;
        unsigned int m_Width, m_Height;
        bool m_VSync;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3WINDOW
