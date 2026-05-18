#ifndef PLATFORM_SDL3_SDL3WINDOW
#define PLATFORM_SDL3_SDL3WINDOW

#include "engine_export.h"

#include "Engine/Platform/IWindow.h"
#include <SDL3/SDL.h>

namespace Engine
{
    class ENGINE_EXPORT SDL3Window : public IWindow {
    public:
        SDL3Window(const WindowProperties& properties);
        ~SDL3Window() override;

        unsigned int GetWidth() override;
        unsigned int GetHeight() override;

        // SDL3Window specific -- not declared in IWindow
        SDL_Window* GetSDLWindow() const;

    private:
        SDL_Window* m_Window;
        unsigned int m_Width, m_Height;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3WINDOW
