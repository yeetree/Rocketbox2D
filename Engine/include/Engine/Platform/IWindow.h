#ifndef ENGINE_PLATFORM_IWINDOW
#define ENGINE_PLATFORM_IWINDOW

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"
#include <string>

namespace Engine
{
    struct WindowProperties {
        std::string title;
        unsigned int width, height;
        GraphicsAPI api;
        bool resizeable;
        bool vsync;
    };

    class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() = 0;
        virtual unsigned int GetHeight() = 0;

        virtual void SetVSync(bool vsync) = 0;
        virtual bool IsVSync() = 0;
    };
} // namespace Engine


#endif // ENGINE_PLATFORM_IWINDOW
