#ifndef ENGINE_PLATFORM_IWINDOW
#define ENGINE_PLATFORM_IWINDOW

#include "engine_export.h"

#include "Engine/Core/Base.h"

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

    class ENGINE_EXPORT IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual unsigned int GetWidth() = 0;
        virtual unsigned int GetHeight() = 0;
    };
} // namespace Engine

#endif // ENGINE_PLATFORM_IWINDOW
