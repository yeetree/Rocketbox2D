#ifndef ENGINE_CORE_INPUT
#define ENGINE_CORE_INPUT

#include "engine_export.h"

#include <cstdint>
#include <bitset>

#include "Engine/Math/Vector.h"

namespace Engine {
    enum class KeyCode : uint16_t;
    enum class MouseButton : uint8_t;
    class Event;

    class ENGINE_EXPORT Input {
    public:
        static bool IsKeyDown(KeyCode key);
        static bool IsKeyJustPressed(KeyCode key);
        static bool IsKeyJustReleased(KeyCode key);

        static Vec2 GetMousePosition();
        static Vec2 GetMouseDelta();

        static bool IsMouseButtonDown(MouseButton button);
        static bool IsMouseButtonJustPressed(MouseButton button);
        static bool IsMouseButtonJustReleased(MouseButton button);

    private:
        friend class Application; 
        static Input* s_Instance;
        void OnEvent(Event& event);

        Vec2 m_PreviousMousePosition;
        Vec2 m_CurrentMousePosition;
        
        std::bitset<512> m_CurrentKeyState;
        std::bitset<512> m_PreviousKeyState;
        std::bitset<8> m_CurrentMouseState;
        std::bitset<8> m_PreviousMouseState;
    };
}

#endif // ENGINE_CORE_INPUT_
