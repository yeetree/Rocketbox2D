#include "Platform/SDL3/SDL3Platform.h"
#include "Platform/SDL3/SDL3Window.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/WindowEvent.h"

namespace Engine
{
    SDL3Platform::SDL3Platform()
    {
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            throw std::runtime_error(std::format("SDL3: SDL could not initialize! Error: {0}", SDL_GetError()));
            return; // Failure
        }

        // Get base path
        const char * basePath = SDL_GetBasePath();

        if(basePath == NULL)
        {
            throw std::runtime_error(std::format("SDL3: Could not get base path! Error: {0}", SDL_GetError()));
            return; // Failure
        }

        m_BasePath = basePath;
    };

    SDL3Platform::~SDL3Platform()
    {
        SDL_Quit();
    }

    void SDL3Platform::PollEvents()
    {
        EventManager* em = Application::Get()->GetServiceLocator()->Get<EventManager>();
        Input* in = Application::Get()->GetServiceLocator()->Get<Input>();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("Quit"), CreateScope<Event>(Hash32("Quit")));
                    }
                    break;
                }
                
                case SDL_EVENT_KEY_DOWN:
                {
                    KeyCode k = SDLToKeyCode(event.key.key);
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("KeyPressed"), CreateScope<KeyPressedEvent>(k, event.key.repeat));
                    }
                    if(in)
                    {
                        in->SetKeyState(k, true);
                    }
                    break;
                }

                case SDL_EVENT_KEY_UP:
                {
                    KeyCode k = SDLToKeyCode(event.key.key);
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("KeyReleased"), CreateScope<KeyReleasedEvent>(k, event.key.repeat));
                    }
                    if(in)
                    {
                        in->SetKeyState(k, false);
                    }
                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    MouseButton k = SDLToMouseButton(event.button.button);
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("MouseButtonPressed"), CreateScope<MouseButtonPressedEvent>(k));
                    }
                    if(in)
                    {
                        in->SetMouseButtonState(k, true);
                    }
                    break;
                }

                case SDL_EVENT_MOUSE_MOTION:
                {
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("MouseMoved"), CreateScope<MouseMovedEvent>(event.motion.xrel, event.motion.yrel));
                    }
                    if(in)
                    {
                        in->SetAxisState(InputAxis::MouseX, event.motion.xrel);
                        in->SetAxisState(InputAxis::MouseY, event.motion.yrel);
                    }
                    break;
                }

                case SDL_EVENT_MOUSE_WHEEL:
                {
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("MouseScrolled"), CreateScope<MouseScrolledEvent>(event.wheel.x, event.wheel.y));
                    }
                    if(in)
                    {
                        in->SetAxisState(InputAxis::ScrollX, event.wheel.x);
                        in->SetAxisState(InputAxis::ScrollY, event.wheel.y);
                    }
                    break;
                }

                case SDL_EVENT_WINDOW_MOVED:
                {
                    if(em)
                    {
                        em->EnqueueEvent(Hash32("WindowMoved"), CreateScope<WindowMovedEvent>(event.window.data1, event.window.data2));
                    }
                    break;
                }

                case SDL_EVENT_WINDOW_RESIZED:
                {
                    if(em)
                    {
                        // Important for graphics, dispatch immediately
                        em->Dispatch(Hash32("WindowResized"), WindowResizedEvent(event.window.data1, event.window.data2));
                    }
                    break;
                }
            }
        }         
    }

    Scope<IWindow> SDL3Platform::CreateWindow(const WindowProperties& properties)
    {
        Scope<SDL3Window> window = CreateScope<SDL3Window>(properties);
        return window;
    }

    KeyCode SDL3Platform::SDLToKeyCode(SDL_Keycode key)
    {
        if (key >= 32 && key <= 126)
        {
            if (key >= 'a' && key <= 'z')
            {
                return static_cast<KeyCode>(key - 'a' + static_cast<uint16_t>(KeyCode::A));
            }
            return static_cast<KeyCode>(key);
        }

        switch (key)
        {
            case SDLK_ESCAPE:       return KeyCode::Escape; break;
            case SDLK_RETURN:       return KeyCode::Enter; break;
            case SDLK_TAB:          return KeyCode::Tab; break;
            case SDLK_BACKSPACE:    return KeyCode::Backspace; break;
            case SDLK_INSERT:       return KeyCode::Insert; break;
            case SDLK_DELETE:       return KeyCode::Delete; break;
            case SDLK_RIGHT:        return KeyCode::Right; break;
            case SDLK_LEFT:         return KeyCode::Left; break;
            case SDLK_DOWN:         return KeyCode::Down; break;
            case SDLK_UP:           return KeyCode::Up; break;
            case SDLK_PAGEUP:       return KeyCode::PageUp; break;
            case SDLK_PAGEDOWN:     return KeyCode::PageDown; break;
            case SDLK_HOME:         return KeyCode::Home; break;
            case SDLK_END:          return KeyCode::End; break;
            case SDLK_CAPSLOCK:     return KeyCode::CapsLock; break;
            case SDLK_SCROLLLOCK:   return KeyCode::ScrollLock; break;
            case SDLK_NUMLOCKCLEAR: return KeyCode::NumLock; break;
            case SDLK_PRINTSCREEN:  return KeyCode::PrintScreen; break;
            case SDLK_PAUSE:        return KeyCode::Pause; break;

            case SDLK_F1:           return KeyCode::F1; break;
            case SDLK_F2:           return KeyCode::F2; break;
            case SDLK_F3:           return KeyCode::F3; break;
            case SDLK_F4:           return KeyCode::F4; break;
            case SDLK_F5:           return KeyCode::F5; break;
            case SDLK_F6:           return KeyCode::F6; break;
            case SDLK_F7:           return KeyCode::F7; break;
            case SDLK_F8:           return KeyCode::F8; break;
            case SDLK_F9:           return KeyCode::F9; break;
            case SDLK_F10:          return KeyCode::F10; break;
            case SDLK_F11:          return KeyCode::F11; break;
            case SDLK_F12:          return KeyCode::F12; break;
            case SDLK_F13:          return KeyCode::F13; break;
            case SDLK_F14:          return KeyCode::F14; break;
            case SDLK_F15:          return KeyCode::F15; break;
            case SDLK_F16:          return KeyCode::F16; break;
            case SDLK_F17:          return KeyCode::F17; break;
            case SDLK_F18:          return KeyCode::F18; break;
            case SDLK_F19:          return KeyCode::F19; break;
            case SDLK_F20:          return KeyCode::F20; break;
            case SDLK_F21:          return KeyCode::F21; break;
            case SDLK_F22:          return KeyCode::F22; break;
            case SDLK_F23:          return KeyCode::F23; break;
            case SDLK_F24:          return KeyCode::F24; break;

            case SDLK_KP_0:         return KeyCode::KP0; break;
            case SDLK_KP_1:         return KeyCode::KP1; break;
            case SDLK_KP_2:         return KeyCode::KP2; break;
            case SDLK_KP_3:         return KeyCode::KP3; break;
            case SDLK_KP_4:         return KeyCode::KP4; break;
            case SDLK_KP_5:         return KeyCode::KP5; break;
            case SDLK_KP_6:         return KeyCode::KP6; break;
            case SDLK_KP_7:         return KeyCode::KP7; break;
            case SDLK_KP_8:         return KeyCode::KP8; break;
            case SDLK_KP_9:         return KeyCode::KP9; break;
            case SDLK_KP_PERIOD:    return KeyCode::KPDecimal; break;
            case SDLK_KP_DIVIDE:    return KeyCode::KPDivide; break;
            case SDLK_KP_MULTIPLY:  return KeyCode::KPMultiply; break;
            case SDLK_KP_MINUS:     return KeyCode::KPSubtract; break;
            case SDLK_KP_PLUS:      return KeyCode::KPAdd; break;
            case SDLK_KP_ENTER:     return KeyCode::KPEnter; break;
            case SDLK_KP_EQUALS:    return KeyCode::KPEqual; break;

            case SDLK_LSHIFT:       return KeyCode::LeftShift; break;
            case SDLK_LCTRL:        return KeyCode::LeftControl; break;
            case SDLK_LALT:         return KeyCode::LeftAlt; break;
            case SDLK_LGUI:         return KeyCode::LeftSuper; break;
            case SDLK_RSHIFT:       return KeyCode::RightShift; break;
            case SDLK_RCTRL:        return KeyCode::RightControl; break;
            case SDLK_RALT:         return KeyCode::RightAlt; break;
            case SDLK_RGUI:         return KeyCode::RightSuper; break;
            case SDLK_MENU:         return KeyCode::Menu; break;

            default:                return KeyCode::None;
        }
    }

    MouseButton SDL3Platform::SDLToMouseButton(uint8_t button)
    {
        switch (button)
        {
            case SDL_BUTTON_LEFT:   return MouseButton::Left;
            case SDL_BUTTON_RIGHT:  return MouseButton::Right;
            case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
            default:                return MouseButton::None;
        }
    }

} // namespace Engine
