#include "Engine/Core/Input.h"

namespace Engine
{
    Input* Input::s_Instance;

    void Input::OnUpdate() {
        m_PreviousKeyState = m_CurrentKeyState;
        m_CurrentKeyState = SDL_GetKeyboardState(nullptr);
        m_PreviousMousePosition = m_CurrentMousePosition;
        m_PreviousMouseButtonState;
        m_CurrentMouseButtonState = SDL_GetMouseState(&m_CurrentMousePosition.x, &m_CurrentMousePosition.y);
    }

    bool Input::IsKeyDown(SDL_Scancode key) {
        return s_Instance->m_CurrentKeyState[key];
    }

    bool Input::IsKeyJustPressed(SDL_Scancode key) {
        return s_Instance->m_CurrentKeyState[key] && !s_Instance->m_PreviousKeyState[key];
    }

    bool Input::IsKeyJustReleased(SDL_Scancode key) {
        return !s_Instance->m_CurrentKeyState[key] && s_Instance->m_PreviousKeyState[key];
    }

    Vec2 Input::GetMousePosition() {
        return s_Instance->m_CurrentMousePosition;
    }

    Vec2 Input::GetMouseDelta() {
        return s_Instance->m_CurrentMousePosition - s_Instance->m_PreviousMousePosition;
    }

    bool Input::IsMouseButtonDown(uint8_t button) {
        return SDL_BUTTON_MASK(button) & s_Instance->m_CurrentMouseButtonState;
    }

    bool Input::IsMouseButtonJustPressed(uint8_t button) {
        return (SDL_BUTTON_MASK(button) & s_Instance->m_CurrentMouseButtonState) && !(SDL_BUTTON_MASK(button) & s_Instance->m_PreviousMouseButtonState);
    }

    bool Input::IsMouseButtonJustReleased(uint8_t button) {
        return !(SDL_BUTTON_MASK(button) & s_Instance->m_CurrentMouseButtonState) && (SDL_BUTTON_MASK(button) & s_Instance->m_PreviousMouseButtonState);
    }

} // namespace Engine
