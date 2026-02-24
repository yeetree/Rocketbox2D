#ifndef ENGINE_PLATFORM_IPLATFORM
#define ENGINE_PLATFORM_IPLATFORM

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"

namespace Engine
{
    // fwd
    class IGraphicsBridge;
    class IWindow;
    class WindowProperties;

    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;

        virtual Scope<IWindow> CreateWindow(const WindowProperties& properties) = 0;
        virtual IGraphicsBridge& GetGraphicsBridge() = 0;

        static Scope<IPlatform> Create(GraphicsAPI api);
    };
    
} // namespace Engine


#endif // ENGINE_PLATFORM_IPLATFORM
