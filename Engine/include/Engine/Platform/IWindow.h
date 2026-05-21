#ifndef ENGINE_PLATFORM_IWINDOW
#define ENGINE_PLATFORM_IWINDOW

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/Platform.h"
#include "Engine/RHI/GraphicsAPI.h"

#include <string>

namespace Engine
{
    struct WindowProperties {
        std::string title;
        unsigned int width, height;
        GraphicsAPI api;
        bool resizable;
    };

    // Represents a game engine window.
    class ENGINE_EXPORT IWindow
    {
    private:
        Platform m_Platform;
        GraphicsAPI m_API;

    protected:
        IWindow(Platform platform, GraphicsAPI api) : m_Platform(platform), m_API(api) {}

    public:
        virtual ~IWindow() = default;

        virtual unsigned int GetWidth() = 0;
        virtual unsigned int GetHeight() = 0;

        GraphicsAPI GetAPI() const { return m_API; }
        Platform GetPlatform() const { return m_Platform; }
    };
} // namespace Engine

#endif // ENGINE_PLATFORM_IWINDOW
