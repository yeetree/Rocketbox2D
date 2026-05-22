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
        Ref<EventManager> em = Application::Get()->GetServiceLocator()->Get<EventManager>();
        Ref<Input> in = Application::Get()->GetServiceLocator()->Get<Input>();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    if(em)
                    {
                        em->QueueEvent(Hash32("Quit"), CreateScope<Event>(Hash32("Quit")));
                    }
                    break;
                }
                
                case SDL_EVENT_KEY_DOWN:
                {
                    KeyCode k = SDLToKeyCode(event.key.key);
                    if(em)
                    {
                        em->QueueEvent(Hash32("KeyPressed"), CreateScope<KeyPressedEvent>(k, event.key.repeat));
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
                        em->QueueEvent(Hash32("KeyReleased"), CreateScope<KeyReleasedEvent>(k, event.key.repeat));
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
                        em->QueueEvent(Hash32("MouseButtonPressed"), CreateScope<MouseButtonPressedEvent>(k));
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
                        em->QueueEvent(Hash32("MouseMoved"), CreateScope<MouseMovedEvent>(event.motion.xrel, event.motion.yrel));
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
                        em->QueueEvent(Hash32("MouseScrolled"), CreateScope<MouseScrolledEvent>(event.wheel.x, event.wheel.y));
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
                        em->QueueEvent(Hash32("WindowMoved"), CreateScope<WindowMovedEvent>(event.window.data1, event.window.data2));
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
            case SDLK_ESCAPE:       return KeyCode::Escape;
            case SDLK_RETURN:       return KeyCode::Enter;
            case SDLK_TAB:          return KeyCode::Tab;
            case SDLK_BACKSPACE:    return KeyCode::Backspace;
            case SDLK_INSERT:       return KeyCode::Insert;
            case SDLK_DELETE:       return KeyCode::Delete;
            case SDLK_RIGHT:        return KeyCode::Right;
            case SDLK_LEFT:         return KeyCode::Left;
            case SDLK_DOWN:         return KeyCode::Down;
            case SDLK_UP:           return KeyCode::Up;
            case SDLK_PAGEUP:       return KeyCode::PageUp;
            case SDLK_PAGEDOWN:     return KeyCode::PageDown;
            case SDLK_HOME:         return KeyCode::Home;
            case SDLK_END:          return KeyCode::End;
            case SDLK_CAPSLOCK:     return KeyCode::CapsLock;
            case SDLK_SCROLLLOCK:   return KeyCode::ScrollLock;
            case SDLK_NUMLOCKCLEAR: return KeyCode::NumLock;
            case SDLK_PRINTSCREEN:  return KeyCode::PrintScreen;
            case SDLK_PAUSE:        return KeyCode::Pause;

            case SDLK_F1:           return KeyCode::F1;
            case SDLK_F2:           return KeyCode::F2;
            case SDLK_F3:           return KeyCode::F3;
            case SDLK_F4:           return KeyCode::F4;
            case SDLK_F5:           return KeyCode::F5;
            case SDLK_F6:           return KeyCode::F6;
            case SDLK_F7:           return KeyCode::F7;
            case SDLK_F8:           return KeyCode::F8;
            case SDLK_F9:           return KeyCode::F9;
            case SDLK_F10:          return KeyCode::F10;
            case SDLK_F11:          return KeyCode::F11;
            case SDLK_F12:          return KeyCode::F12;
            case SDLK_F13:          return KeyCode::F13;
            case SDLK_F14:          return KeyCode::F14;
            case SDLK_F15:          return KeyCode::F15;
            case SDLK_F16:          return KeyCode::F16;
            case SDLK_F17:          return KeyCode::F17;
            case SDLK_F18:          return KeyCode::F18;
            case SDLK_F19:          return KeyCode::F19;
            case SDLK_F20:          return KeyCode::F20;
            case SDLK_F21:          return KeyCode::F21;
            case SDLK_F22:          return KeyCode::F22;
            case SDLK_F23:          return KeyCode::F23;
            case SDLK_F24:          return KeyCode::F24;

            case SDLK_KP_0:         return KeyCode::KP0;
            case SDLK_KP_1:         return KeyCode::KP1;
            case SDLK_KP_2:         return KeyCode::KP2;
            case SDLK_KP_3:         return KeyCode::KP3;
            case SDLK_KP_4:         return KeyCode::KP4;
            case SDLK_KP_5:         return KeyCode::KP5;
            case SDLK_KP_6:         return KeyCode::KP6;
            case SDLK_KP_7:         return KeyCode::KP7;
            case SDLK_KP_8:         return KeyCode::KP8;
            case SDLK_KP_9:         return KeyCode::KP9;
            case SDLK_KP_PERIOD:    return KeyCode::KPDecimal;
            case SDLK_KP_DIVIDE:    return KeyCode::KPDivide;
            case SDLK_KP_MULTIPLY:  return KeyCode::KPMultiply;
            case SDLK_KP_MINUS:     return KeyCode::KPSubtract;
            case SDLK_KP_PLUS:      return KeyCode::KPAdd;
            case SDLK_KP_ENTER:     return KeyCode::KPEnter;
            case SDLK_KP_EQUALS:    return KeyCode::KPEqual;

            case SDLK_LSHIFT:       return KeyCode::LeftShift;
            case SDLK_LCTRL:        return KeyCode::LeftControl;
            case SDLK_LALT:         return KeyCode::LeftAlt;
            case SDLK_LGUI:         return KeyCode::LeftSuper;
            case SDLK_RSHIFT:       return KeyCode::RightShift;
            case SDLK_RCTRL:        return KeyCode::RightControl;
            case SDLK_RALT:         return KeyCode::RightAlt;
            case SDLK_RGUI:         return KeyCode::RightSuper;
            case SDLK_MENU:         return KeyCode::Menu;

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
