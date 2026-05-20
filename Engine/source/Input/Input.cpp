#include "Engine/Input/Input.h"

#include <cstring>

namespace Engine
{
    void Input::SetKeyState(KeyCode key, bool pressed)
    {
        m_KeyStateCurrent[(uint16_t)key] = pressed;
    }

    void Input::SetMouseButtonState(MouseButton button, bool pressed)
    {
        m_MouseButtonStateCurrent[(uint8_t)button] = pressed;
    }

    void Input::SetAxisState(InputAxis axis, float value)
    {
        m_AxisStateCurrent[(uint8_t)axis] = value;
    }

    void Input::Update()
    {
        m_KeyStatePrevious = m_KeyStateCurrent;
        m_MouseButtonStatePrevious = m_MouseButtonStateCurrent;
        m_AxisStatePrevious = m_AxisStateCurrent;

        m_AxisStateCurrent[(uint8_t)InputAxis::MouseX] = 0.0f;
        m_AxisStateCurrent[(uint8_t)InputAxis::MouseY] = 0.0f;
        m_AxisStateCurrent[(uint8_t)InputAxis::ScrollX] = 0.0f;
        m_AxisStateCurrent[(uint8_t)InputAxis::ScrollY] = 0.0f;
    }

    // Action mapping
    void Input::MapAction(const StringName& action, KeyCode negative, KeyCode positive)
    {
        m_ActionMap[action] = Action{
            .type = ActionType::Digital,
            .button = InputButton::Key,
            .positiveKey = positive,
            .negativeKey = negative
        };
    }

    void Input::MapAction(const StringName& action, KeyCode positive)
    {
        MapAction(action, KeyCode::None, positive);
    }

    void Input::MapAction(const StringName& action, MouseButton negative, MouseButton positive)
    {
        m_ActionMap[action] = Action{
            .type = ActionType::Digital,
            .button = InputButton::MouseButton,
            .positiveMB = positive,
            .negativeMB = negative
        };
    }

    void Input::MapAction(const StringName& action, MouseButton positive)
    {
        MapAction(action, MouseButton::None, positive);
    }

    void Input::MapAction(const StringName& action, InputAxis axis)
    {
        m_ActionMap[action] = Action{
            .type = ActionType::Analog,
            .axis = axis,
        };
    }

    // Getters
    bool Input::IsKeyDown(KeyCode key)
    {
        return m_KeyStateCurrent[(uint16_t)key];
    }

    bool Input::IsKeyPressed(KeyCode key)
    {
        return m_KeyStateCurrent[(uint16_t)key] && !m_KeyStatePrevious[(uint16_t)key];
    }

    bool Input::IsKeyReleased(KeyCode key)
    {
        return !m_KeyStateCurrent[(uint16_t)key] && m_KeyStatePrevious[(uint16_t)key];
    }

    bool Input::IsMouseButtonDown(MouseButton mb)
    {
        return m_MouseButtonStateCurrent[(uint8_t)mb];
    }

    bool Input::IsMouseButtonPressed(MouseButton mb)
    {
        return m_MouseButtonStateCurrent[(uint8_t)mb] && !m_MouseButtonStatePrevious[(uint8_t)mb];
    }

    bool Input::IsMouseButtonReleased(MouseButton mb)
    {
        return !m_MouseButtonStateCurrent[(uint8_t)mb] && m_MouseButtonStatePrevious[(uint8_t)mb];
    }

    float Input::GetAxis(InputAxis axis)
    {
        return m_AxisStateCurrent[(uint8_t)axis];
    }

    float Input::GetAxisDelta(InputAxis axis)
    {
        return m_AxisStateCurrent[(uint8_t)axis] - m_AxisStatePrevious[(uint8_t)axis];
    }

    bool Input::IsActionDown(const StringName& action)
    {
        auto actionIt = m_ActionMap.find(action);
        if(actionIt != m_ActionMap.end())
        {
            if(actionIt->second.type == ActionType::Analog)
            {
                return GetAxis(actionIt->second.axis) != 0.0f;
            }
            else if(actionIt->second.type == ActionType::Digital)
            {
                if(actionIt->second.button == InputButton::Key)
                {
                    return IsKeyDown(actionIt->second.positiveKey) || IsKeyDown(actionIt->second.negativeKey);
                }
                else if(actionIt->second.button == InputButton::MouseButton)
                {
                    return IsMouseButtonDown(actionIt->second.positiveMB) || IsMouseButtonDown(actionIt->second.negativeMB);
                }
            }
        }
        return false;
    }

    bool Input::IsActionPressed(const StringName& action)
    {
        auto actionIt = m_ActionMap.find(action);
        if(actionIt != m_ActionMap.end())
        {
            if(actionIt->second.type == ActionType::Analog)
            {
                return (GetAxis(actionIt->second.axis) != 0.0f) && (m_AxisStatePrevious[(uint8_t)actionIt->second.axis] == 0.0f);
            }
            else if(actionIt->second.type == ActionType::Digital)
            {
                if(actionIt->second.button == InputButton::Key)
                {
                    return IsKeyPressed(actionIt->second.positiveKey) || IsKeyPressed(actionIt->second.negativeKey);
                }
                else if(actionIt->second.button == InputButton::MouseButton)
                {
                    return IsMouseButtonPressed(actionIt->second.positiveMB) || IsMouseButtonPressed(actionIt->second.negativeMB);
                }
            }
        }
        return false;
    }

    bool Input::IsActionReleased(const StringName& action)
    {
        auto actionIt = m_ActionMap.find(action);
        if(actionIt != m_ActionMap.end())
        {
            if(actionIt->second.type == ActionType::Analog)
            {
                return (GetAxis(actionIt->second.axis) == 0.0f) && (m_AxisStatePrevious[(uint8_t)actionIt->second.axis] != 0.0f);
            }
            else if(actionIt->second.type == ActionType::Digital)
            {
                if(actionIt->second.button == InputButton::Key)
                {
                    return IsKeyReleased(actionIt->second.positiveKey) || IsKeyReleased(actionIt->second.negativeKey);
                }
                else if(actionIt->second.button == InputButton::MouseButton)
                {
                    return IsMouseButtonPressed(actionIt->second.positiveMB) || IsMouseButtonPressed(actionIt->second.negativeMB);
                }
            }
        }
        return false;
    }

    float Input::GetAction(const StringName& action)
    {
        auto actionIt = m_ActionMap.find(action);
        if(actionIt != m_ActionMap.end())
        {
            if(actionIt->second.type == ActionType::Analog)
            {
                return GetAxis(actionIt->second.axis);
            }
            else if(actionIt->second.type == ActionType::Digital)
            {
                if(actionIt->second.button == InputButton::Key)
                {
                    return (1.0f * IsKeyDown(actionIt->second.positiveKey)) - (1.0f * IsKeyDown(actionIt->second.negativeKey));
                }
                else if(actionIt->second.button == InputButton::MouseButton)
                {
                    return (1.0f * IsMouseButtonDown(actionIt->second.positiveMB)) - (1.0f * IsMouseButtonDown(actionIt->second.negativeMB));
                }
            }
        }
        return 0.0f;
    }

    float Input::GetActionDelta(const StringName& action)
    {
        auto actionIt = m_ActionMap.find(action);
        if(actionIt != m_ActionMap.end())
        {
            if(actionIt->second.type == ActionType::Analog)
            {
                return GetAxisDelta(actionIt->second.axis);
            }
            else if(actionIt->second.type == ActionType::Digital)
            {
                if(actionIt->second.button == InputButton::Key)
                {
                    float current = (1.0f * IsKeyDown(actionIt->second.positiveKey)) - (1.0f * IsKeyDown(actionIt->second.negativeKey));
                    float prev = (1.0f * m_KeyStatePrevious[(uint16_t)actionIt->second.positiveKey]) - (1.0f *  m_KeyStatePrevious[(uint16_t)actionIt->second.negativeKey]);
                    return current - prev;
                }
                else if(actionIt->second.button == InputButton::MouseButton)
                {
                    float current = (1.0f * IsMouseButtonDown(actionIt->second.positiveMB)) - (1.0f * IsMouseButtonDown(actionIt->second.negativeMB));
                    float prev = (1.0f * m_MouseButtonStatePrevious[(uint8_t)actionIt->second.positiveMB]) - (1.0f *  m_MouseButtonStatePrevious[(uint8_t)actionIt->second.negativeMB]);
                    return current - prev;
                }
            }
        }
        return 0.0f;
    }
} // namespace Engine
