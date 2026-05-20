#ifndef PLATFORM_SDL3_SDL3PLATFORM
#define PLATFORM_SDL3_SDL3PLATFORM

#include "engine_export.h"

#include "Engine/Platform/IPlatform.h"

#include "Engine/Input/KeyCode.h"
#include "Engine/Input/MouseButton.h"

#include <SDL3/SDL.h>

#include <string>
#include <functional>
#include <unordered_map>
#include <array>

namespace Engine
{
    class ENGINE_EXPORT SDL3Platform : public IPlatform {     
    private:
        KeyCode SDLToKeyCode(SDL_Keycode key);
        MouseButton SDLToMouseButton(uint8_t button);

    public:
        SDL3Platform();
        ~SDL3Platform() override;
        
        void PollEvents() override;

        // Resource creation
        Scope<IWindow> CreateWindow(const WindowProperties& properties) override;
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3PLATFORM