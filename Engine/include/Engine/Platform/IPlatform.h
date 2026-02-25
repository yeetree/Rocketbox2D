#ifndef ENGINE_PLATFORM_IPLATFORM
#define ENGINE_PLATFORM_IPLATFORM

#include <string>
#include <cstdint>
#include <functional>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/GraphicsAPI.h"

namespace Engine
{
    // fwd
    class IGraphicsBridge;
    class IWindow;
    class WindowProperties;
    class Event;

    class IPlatform
    {
    public:
        virtual ~IPlatform() = default;

        // Events
        virtual void PollEvents() = 0;
        virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;

        // Resource creation
        virtual Scope<IWindow> CreateWindow(const WindowProperties& properties) = 0;

        // Getters
        virtual const std::string& GetBasePath() const = 0;
        virtual uint64_t GetTicks() const = 0;
        virtual uint64_t GetTicksNS() const = 0;
        virtual double GetTime() const = 0;
        virtual IGraphicsBridge& GetGraphicsBridge() = 0;
        

        static Scope<IPlatform> Create(GraphicsAPI api);
    };
    
} // namespace Engine


#endif // ENGINE_PLATFORM_IPLATFORM
