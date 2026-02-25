#ifndef ENGINE_PLATFORM_IWINDOW
#define ENGINE_PLATFORM_IWINDOW

#include <string>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"

namespace Engine
{
    struct WindowProperties {
        std::string title;
        unsigned int width, height;
        GraphicsAPI api;
        bool resizeable;
    };

    class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual unsigned int GetWidth() = 0;
        virtual unsigned int GetHeight() = 0;
    };
} // namespace Engine


#endif // ENGINE_PLATFORM_IWINDOW
