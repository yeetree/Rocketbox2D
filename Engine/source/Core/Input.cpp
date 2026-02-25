#include "Engine/Core/Input.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine
{
    Input* Input::s_Instance;

    void Input::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& event){
            if(!event.IsRepeat()) {
                uint16_t code = (uint16_t)event.GetKeyCode();
                m_PreviousKeyState[code] = m_CurrentKeyState[code];
                m_CurrentKeyState[code] = true;
            }
            return false; // we are generous
        });

        dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& event){
            uint16_t code = (uint16_t)event.GetKeyCode();
            m_PreviousKeyState[code] = m_CurrentKeyState[code];
            m_CurrentKeyState[code] = true;
            return false;
        });

        dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& event){
            uint8_t button = (uint8_t)event.GetMouseButton();
            m_PreviousMouseState[button] = m_CurrentMouseState[button];
            m_CurrentMouseState[button] = true;
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& event){
            uint8_t button = (uint8_t)event.GetMouseButton();
            m_PreviousMouseState[button] = m_CurrentMouseState[button];
            m_CurrentMouseState[button] = false;
            return false;
        });

        dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& event){
            m_PreviousMousePosition = m_CurrentMousePosition;
            m_CurrentMousePosition.x = event.GetX();
            m_CurrentMousePosition.y = event.GetY();
            return false;
        });
        
    }

    bool Input::IsKeyDown(KeyCode key) {
        return s_Instance->m_CurrentKeyState[(uint16_t)key];
    }

    bool Input::IsKeyJustPressed(KeyCode key) {
        return s_Instance->m_CurrentKeyState[(uint16_t)key] && !s_Instance->m_PreviousKeyState[(uint16_t)key];
    }

    bool Input::IsKeyJustReleased(KeyCode key) {
        return !s_Instance->m_CurrentKeyState[(uint16_t)key] && s_Instance->m_PreviousKeyState[(uint16_t)key];
    }

    Vec2 Input::GetMousePosition() {
        return s_Instance->m_CurrentMousePosition;
    }

    Vec2 Input::GetMouseDelta() {
        return s_Instance->m_CurrentMousePosition - s_Instance->m_PreviousMousePosition;
    }

    bool Input::IsMouseButtonDown(MouseButton button) {
        return s_Instance->m_CurrentMouseState[(uint8_t)button];
    }

    bool Input::IsMouseButtonJustPressed(MouseButton button) {
        return s_Instance->m_CurrentMouseState[(uint8_t)button] && !s_Instance->m_PreviousMouseState[(uint8_t)button];
    }

    bool Input::IsMouseButtonJustReleased(MouseButton button) {
        return !s_Instance->m_CurrentMouseState[(uint8_t)button] && s_Instance->m_PreviousMouseState[(uint8_t)button];
    }

} // namespace Engine
