#ifndef ENGINE_CORE_INPUT
#define ENGINE_CORE_INPUT

#include "engine_export.h"

#include <SDL3/SDL.h>
#include <cstdint>

#include "Engine/Math/Vector.h"

namespace Engine {
    class ENGINE_EXPORT Input {
    public:
        static bool IsKeyDown(SDL_Scancode key);
        static bool IsKeyJustPressed(SDL_Scancode key);
        static bool IsKeyJustReleased(SDL_Scancode key);

        static Vec2 GetMousePosition();
        static Vec2 GetMouseDelta();

        static bool IsMouseButtonDown(uint8_t button);
        static bool IsMouseButtonJustPressed(uint8_t button);
        static bool IsMouseButtonJustReleased(uint8_t button);

    private:
        friend class Application; 
        static Input* s_Instance;
        void OnUpdate();

        Vec2 m_PreviousMousePosition;
        Vec2 m_CurrentMousePosition;
        uint32_t m_PreviousMouseButtonState;
        uint32_t m_CurrentMouseButtonState;
        const bool* m_PreviousKeyState;
        const bool* m_CurrentKeyState;
    };
}

#endif // ENGINE_CORE_INPUT_
