#ifndef PLATFORM_SDL3_SDL3PLATFORM
#define PLATFORM_SDL3_SDL3PLATFORM

#include "Engine/Platform/IPlatform.h"
#include "Engine/Platform/IGraphicsBridge.h"

#include <SDL3/SDL.h>
#include <functional>
#include <unordered_map>

namespace Engine
{
    // fwd:
    class SDL3Window;
    enum class KeyCode : uint16_t;
    enum class MouseButton : uint8_t;

    extern const std::array<KeyCode, SDL_SCANCODE_COUNT> SDLToKeyCode;
    extern const std::array<MouseButton, 6> SDLToMouseButton;

    class SDL3Platform : public IPlatform {
    public:
        SDL3Platform(Scope<IGraphicsBridge> graphicsBridge);
        ~SDL3Platform() override;

        // Events
        void PollEvents() override;
        void SetEventCallback(const std::function<void(Event&)>& callback) override;

        // Resource creation
        Scope<IWindow> CreateWindow(const WindowProperties& properties) override;

        // Getters
        const std::string& GetBasePath() const override;
        uint64_t GetTicks() const override;
        uint64_t GetTicksNS() const override;
        double GetTime() const override;
        IGraphicsBridge& GetGraphicsBridge() override;

    private:
        friend class SDL3Window; // we allow SDL3Window to access m_WindowMap to add and remove itself

        Scope<IGraphicsBridge> m_GraphicsBridge;

        // maps SDL's windowID in events to SDL3Window so we can update its internal
        // data
        std::unordered_map<uint32_t, SDL3Window*> m_WindowMap;

        std::function<void(Event&)> m_Callback;

        std::string m_BasePath;

        
    };
} // namespace Engine


#endif // PLATFORM_SDL3_SDL3PLATFORM
