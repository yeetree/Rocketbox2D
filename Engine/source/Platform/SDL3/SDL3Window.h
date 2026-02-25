#ifndef PLATFORM_SDL3_SDL3WINDOW
#define PLATFORM_SDL3_SDL3WINDOW

#include "Engine/Platform/IWindow.h"
#include <SDL3/SDL.h>

namespace Engine
{
    // fwd
    class SDL3Platform;

    class SDL3Window : public IWindow {
    public:
        SDL3Window(SDL3Platform* platform, const WindowProperties& properties);
        ~SDL3Window() override;

        unsigned int GetWidth() override;
        unsigned int GetHeight() override;

        // Getters for SDL3*GraphicsBridge classes (not declared in IWindow)
        SDL_Window* GetSDLWindow();

    private:
        friend class SDL3Platform; // We allow SDL3Platform to update our members

        SDL3Platform* m_Platform;
        SDL_Window* m_Window;
        uint32_t m_SDLID;
        unsigned int m_Width, m_Height;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3WINDOW
