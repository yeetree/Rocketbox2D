#ifndef PLATFORM_SDL3_SDL3PLATFORM
#define PLATFORM_SDL3_SDL3PLATFORM

#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IGraphicsBridge.h"

namespace Engine
{
    class SDL3Platform : public IPlatform {
    public:
        SDL3Platform(Scope<IGraphicsBridge> graphicsBridge);
        ~SDL3Platform() override;

        Scope<IWindow> CreateWindow(const WindowProperties& properties) override;
        IGraphicsBridge& GetGraphicsBridge() override;

    private:
        Scope<IGraphicsBridge> m_GraphicsBridge;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3PLATFORM
