#ifndef ENGINE_INPUT_INPUT
#define ENGINE_INPUT_INPUT

#include "engine_export.h"

#include "Engine/Core/Base.h"

#include "Engine/Core/StringName.h"

#include "Engine/Input/KeyCode.h"
#include "Engine/Input/MouseButton.h"
#include "Engine/Input/InputAxis.h"
#include "Engine/Input/InputButton.h"

#include <unordered_map>
#include <bitset>
#include <array>

namespace Engine
{
    class ENGINE_EXPORT Input
    {
    private:
        enum class ActionType
        {
            None = 0,
            Digital = 1,
            Analog = 2,
        };

        struct Action
        {
            ActionType type = ActionType::None;
            
            InputButton button = InputButton::None;

            KeyCode positiveKey = KeyCode::None;
            KeyCode negativeKey = KeyCode::None;

            MouseButton positiveMB = MouseButton::None;
            MouseButton negativeMB = MouseButton::None;

            InputAxis axis = InputAxis::None;
        };

        // State
        std::bitset<MaxKeyCode+1> m_KeyStateCurrent;
        std::bitset<MaxKeyCode+1> m_KeyStatePrevious;

        std::bitset<MaxMouseButton+1> m_MouseButtonStateCurrent;
        std::bitset<MaxMouseButton+1> m_MouseButtonStatePrevious;

        std::array<float, MaxInputAxis+1> m_AxisStateCurrent;
        std::array<float, MaxInputAxis+1> m_AxisStatePrevious;

        // Action map
        std::unordered_map<uint32_t, Action> m_ActionMap;

    public:
        Input() = default;

        // State setters
        void SetKeyState(KeyCode key, bool pressed);
        void SetMouseButtonState(MouseButton button, bool pressed);
        void SetAxisState(InputAxis axis, float value);
        void Update();

        // Action mapping
        void MapAction(const StringName& action, KeyCode negative, KeyCode positive);
        void MapAction(const StringName& action, KeyCode positive);
        void MapAction(const StringName& action, MouseButton negative, MouseButton positive);
        void MapAction(const StringName& action, MouseButton positive);
        void MapAction(const StringName& action, InputAxis axis);

        // Getters
        bool IsKeyDown(KeyCode key);
        bool IsKeyPressed(KeyCode key);
        bool IsKeyReleased(KeyCode key);
        bool IsMouseButtonDown(MouseButton key);
        bool IsMouseButtonPressed(MouseButton key);
        bool IsMouseButtonReleased(MouseButton key);
        float GetAxis(InputAxis axis);
        float GetAxisDelta(InputAxis axis);

        bool IsActionDown(const StringName& action);
        bool IsActionPressed(const StringName& action);
        bool IsActionReleased(const StringName& action);
        float GetAction(const StringName& action);
        float GetActionDelta(const StringName& action);
    };
} // namespace Engine


#endif // ENGINE_INPUT_INPUT
