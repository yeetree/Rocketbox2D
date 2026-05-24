#ifndef ENGINE_PLATFORM_IGRAPHICSBRIDGE
#define ENGINE_PLATFORM_IGRAPHICSBRIDGE

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Platform/Platform.h"
#include "Engine/RHI/GraphicsAPI.h"

namespace Engine
{
    // Helper class to provide API-specific functions that rely on platform-specific implementations.
    class ENGINE_EXPORT IGraphicsBridge
    {
    private:
        Platform m_Platform;
        GraphicsAPI m_API;

    protected:
        IGraphicsBridge(Platform platform, GraphicsAPI api) : m_Platform(platform), m_API(api) {}

    public:
        virtual ~IGraphicsBridge() = default;

        Platform GetPlatform() const { return m_Platform; }
        GraphicsAPI GetAPI() const { return m_API; }
    };
} // namespace Engine


#endif // ENGINE_PLATFORM_IGRAPHICSBRIDGE
