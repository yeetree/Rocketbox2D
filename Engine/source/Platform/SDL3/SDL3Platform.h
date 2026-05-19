#ifndef PLATFORM_SDL3_SDL3PLATFORM
#define PLATFORM_SDL3_SDL3PLATFORM

#include "engine_export.h"

#include "Engine/Platform/IPlatform.h"

#include <SDL3/SDL.h>

#include <string>
#include <functional>
#include <unordered_map>
#include <array>

namespace Engine
{
    class ENGINE_EXPORT SDL3Platform : public IPlatform {     
    public:
        SDL3Platform();
        ~SDL3Platform() override;
        
        void PollEvents() override;

        // Resource creation
        Scope<IWindow> CreateWindow(const WindowProperties& properties) override;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3PLATFORM