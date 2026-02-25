#include "Platform/SDL3/SDL3Platform.h"
#include "Platform/SDL3/SDL3Window.h"

#include "Engine/Core/Log.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"

#include <SDL3/SDL.h>
#include <format>

namespace Engine
{
    SDL3Platform::SDL3Platform(Scope<IGraphicsBridge> graphicsBridge)
        : m_GraphicsBridge(std::move(graphicsBridge)) 
    {
        // Initialize SDL
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

    SDL3Platform::~SDL3Platform() {
        SDL_Quit();
    }

    void SDL3Platform::PollEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            // Get window of event
            SDL3Window* eventWindow = nullptr;

            // Get SDL3Window from event
            SDL_Window* sdlWindow = SDL_GetWindowFromEvent(&event);
            if (sdlWindow != NULL) {
                uint32_t windowID = SDL_GetWindowID(sdlWindow);
                auto it = m_WindowMap.find(windowID);
                if (it != m_WindowMap.end()) {
                    eventWindow = it->second;
                }
            }

            switch (event.type) {
                // Window
                case SDL_EVENT_QUIT: {
                    QuitEvent e;
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                    if(eventWindow == nullptr) { break; }
                    WindowClosedEvent e(eventWindow);
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    if(eventWindow == nullptr) { break; }
                    eventWindow->m_Width = event.window.data1;
                    eventWindow->m_Height = event.window.data2;
                    WindowResizedEvent e(eventWindow, event.window.data1, event.window.data2);
                    m_Callback(e);
                    break;
                }
                // Keyboard
                case SDL_EVENT_KEY_DOWN: {
                    if(eventWindow == nullptr) { break; }
                    // SDL3 keycode maps 1:1 with KeyCode
                    KeyPressedEvent e(eventWindow, SDLToKeyCode[event.key.scancode], event.key.repeat);
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_KEY_UP: {
                    if(eventWindow == nullptr) { break; }
                    // SDL3 keycode maps 1:1 with KeyCode
                    KeyReleasedEvent e(eventWindow, SDLToKeyCode[event.key.scancode]);
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                    if(eventWindow == nullptr) { break; }
                    // SDL3 mouse code maps 1:1 with MouseCode
                    MouseButtonPressedEvent e(eventWindow, SDLToMouseButton[event.button.button]);
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_MOUSE_BUTTON_UP: {
                    if(eventWindow == nullptr) { break; }
                    // SDL3 mouse code maps 1:1 with MouseCode
                    MouseButtonReleasedEvent e(eventWindow, SDLToMouseButton[event.button.button]);
                    m_Callback(e);
                    break;
                }
                case SDL_EVENT_MOUSE_MOTION: {
                    if(eventWindow == nullptr) { break; }
                    MouseMovedEvent e(eventWindow, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                    m_Callback(e);
                    break;
                };
                case SDL_EVENT_MOUSE_WHEEL: {
                    if(eventWindow == nullptr) { break; }
                    MouseWheelScrolledEvent e(eventWindow, event.wheel.x, event.wheel.y);
                    m_Callback(e);
                    break;
                };
            }            
        }
    }

    void SDL3Platform::SetEventCallback(const std::function<void(Event&)>& callback) {
        m_Callback = callback;
    }

    Scope<IWindow> SDL3Platform::CreateWindow(const WindowProperties& properties) {
        // Create window (as SDL3Window)
        Scope<SDL3Window> window = CreateScope<SDL3Window>(this, properties);

        // Return IWindow
        return window;
    }

    const std::string& SDL3Platform::GetBasePath() const {
        return m_BasePath;
    }

    uint64_t SDL3Platform::GetTicks() const {
        return SDL_GetTicks();
    }

    uint64_t SDL3Platform::GetTicksNS() const {
        return SDL_GetTicksNS();
    }

    double SDL3Platform::GetTime() const {
        return static_cast<double>(GetTicksNS()) / 1000000000.0;
    }

    IGraphicsBridge& SDL3Platform::GetGraphicsBridge() {
        return *m_GraphicsBridge;
    }

    // Populate array
    const std::array<KeyCode, SDL_SCANCODE_COUNT> SDLToKeyCode = [] {
        std::array<KeyCode, SDL_SCANCODE_COUNT> mapping;
        mapping.fill(KeyCode::None);

        mapping[SDL_SCANCODE_SPACE]          = KeyCode::Space;
        mapping[SDL_SCANCODE_APOSTROPHE]     = KeyCode::Apostrophe;
        mapping[SDL_SCANCODE_COMMA]          = KeyCode::Comma;
        mapping[SDL_SCANCODE_MINUS]          = KeyCode::Minus;
        mapping[SDL_SCANCODE_PERIOD]         = KeyCode::Period;
        mapping[SDL_SCANCODE_SLASH]          = KeyCode::Slash;
        mapping[SDL_SCANCODE_SEMICOLON]      = KeyCode::Semicolon;
        mapping[SDL_SCANCODE_EQUALS]         = KeyCode::Equal;
        mapping[SDL_SCANCODE_LEFTBRACKET]    = KeyCode::LeftBracket;
        mapping[SDL_SCANCODE_BACKSLASH]      = KeyCode::Backslash;
        mapping[SDL_SCANCODE_RIGHTBRACKET]   = KeyCode::RightBracket;
        mapping[SDL_SCANCODE_GRAVE]          = KeyCode::GraveAccent;


        mapping[SDL_SCANCODE_0]              = KeyCode::D0;
        mapping[SDL_SCANCODE_1]              = KeyCode::D1;
        mapping[SDL_SCANCODE_2]              = KeyCode::D2;
        mapping[SDL_SCANCODE_3]              = KeyCode::D3;
        mapping[SDL_SCANCODE_4]              = KeyCode::D4;
        mapping[SDL_SCANCODE_5]              = KeyCode::D5;
        mapping[SDL_SCANCODE_6]              = KeyCode::D6;
        mapping[SDL_SCANCODE_7]              = KeyCode::D7;
        mapping[SDL_SCANCODE_8]              = KeyCode::D8;
        mapping[SDL_SCANCODE_9]              = KeyCode::D9;

        mapping[SDL_SCANCODE_A]              = KeyCode::A;
        mapping[SDL_SCANCODE_B]              = KeyCode::B;
        mapping[SDL_SCANCODE_C]              = KeyCode::C;
        mapping[SDL_SCANCODE_D]              = KeyCode::D;
        mapping[SDL_SCANCODE_E]              = KeyCode::E;
        mapping[SDL_SCANCODE_F]              = KeyCode::F;
        mapping[SDL_SCANCODE_G]              = KeyCode::G;
        mapping[SDL_SCANCODE_H]              = KeyCode::H;
        mapping[SDL_SCANCODE_I]              = KeyCode::I;
        mapping[SDL_SCANCODE_J]              = KeyCode::J;
        mapping[SDL_SCANCODE_K]              = KeyCode::K;
        mapping[SDL_SCANCODE_L]              = KeyCode::L;
        mapping[SDL_SCANCODE_M]              = KeyCode::M;
        mapping[SDL_SCANCODE_N]              = KeyCode::N;
        mapping[SDL_SCANCODE_O]              = KeyCode::O;
        mapping[SDL_SCANCODE_P]              = KeyCode::P;
        mapping[SDL_SCANCODE_Q]              = KeyCode::Q;
        mapping[SDL_SCANCODE_R]              = KeyCode::R;
        mapping[SDL_SCANCODE_S]              = KeyCode::S;
        mapping[SDL_SCANCODE_T]              = KeyCode::T;
        mapping[SDL_SCANCODE_U]              = KeyCode::U;
        mapping[SDL_SCANCODE_V]              = KeyCode::V;
        mapping[SDL_SCANCODE_W]              = KeyCode::W;
        mapping[SDL_SCANCODE_X]              = KeyCode::X;
        mapping[SDL_SCANCODE_Y]              = KeyCode::Y;
        mapping[SDL_SCANCODE_Z]              = KeyCode::Z;

        mapping[SDL_SCANCODE_ESCAPE]         = KeyCode::Escape;
        mapping[SDL_SCANCODE_RETURN]         = KeyCode::Enter;
        mapping[SDL_SCANCODE_TAB]            = KeyCode::Tab;
        mapping[SDL_SCANCODE_BACKSPACE]      = KeyCode::Backspace;
        mapping[SDL_SCANCODE_INSERT]         = KeyCode::Insert;
        mapping[SDL_SCANCODE_DELETE]         = KeyCode::Delete;
        mapping[SDL_SCANCODE_RIGHT]          = KeyCode::Right;
        mapping[SDL_SCANCODE_LEFT]           = KeyCode::Left;
        mapping[SDL_SCANCODE_DOWN]           = KeyCode::Down;
        mapping[SDL_SCANCODE_UP]             = KeyCode::Up;
        mapping[SDL_SCANCODE_PAGEUP]         = KeyCode::PageUp;
        mapping[SDL_SCANCODE_PAGEDOWN]       = KeyCode::PageDown;
        mapping[SDL_SCANCODE_HOME]           = KeyCode::Home;
        mapping[SDL_SCANCODE_END]            = KeyCode::End;
        mapping[SDL_SCANCODE_CAPSLOCK]       = KeyCode::CapsLock;
        mapping[SDL_SCANCODE_SCROLLLOCK]     = KeyCode::ScrollLock;
        mapping[SDL_SCANCODE_NUMLOCKCLEAR]   = KeyCode::NumLock;
        mapping[SDL_SCANCODE_PRINTSCREEN]    = KeyCode::PrintScreen;
        mapping[SDL_SCANCODE_PAUSE]          = KeyCode::Pause;

        mapping[SDL_SCANCODE_F1]             = KeyCode::F1;
        mapping[SDL_SCANCODE_F2]             = KeyCode::F2;
        mapping[SDL_SCANCODE_F3]             = KeyCode::F3;
        mapping[SDL_SCANCODE_F4]             = KeyCode::F4;
        mapping[SDL_SCANCODE_F5]             = KeyCode::F5;
        mapping[SDL_SCANCODE_F6]             = KeyCode::F6;
        mapping[SDL_SCANCODE_F7]             = KeyCode::F7;
        mapping[SDL_SCANCODE_F8]             = KeyCode::F8;
        mapping[SDL_SCANCODE_F9]             = KeyCode::F9;
        mapping[SDL_SCANCODE_F10]            = KeyCode::F10;
        mapping[SDL_SCANCODE_F11]            = KeyCode::F11;
        mapping[SDL_SCANCODE_F12]            = KeyCode::F12;

        mapping[SDL_SCANCODE_KP_0]           = KeyCode::KP0;
        mapping[SDL_SCANCODE_KP_1]           = KeyCode::KP1;
        mapping[SDL_SCANCODE_KP_2]           = KeyCode::KP2;
        mapping[SDL_SCANCODE_KP_3]           = KeyCode::KP3;
        mapping[SDL_SCANCODE_KP_4]           = KeyCode::KP4;
        mapping[SDL_SCANCODE_KP_5]           = KeyCode::KP5;
        mapping[SDL_SCANCODE_KP_6]           = KeyCode::KP6;
        mapping[SDL_SCANCODE_KP_7]           = KeyCode::KP7;
        mapping[SDL_SCANCODE_KP_8]           = KeyCode::KP8;
        mapping[SDL_SCANCODE_KP_9]           = KeyCode::KP9;
        mapping[SDL_SCANCODE_KP_PERIOD]      = KeyCode::KPDecimal;
        mapping[SDL_SCANCODE_KP_DIVIDE]      = KeyCode::KPDivide;
        mapping[SDL_SCANCODE_KP_MULTIPLY]    = KeyCode::KPMultiply;
        mapping[SDL_SCANCODE_KP_MINUS]       = KeyCode::KPSubtract;
        mapping[SDL_SCANCODE_KP_PLUS]        = KeyCode::KPAdd;
        mapping[SDL_SCANCODE_KP_ENTER]       = KeyCode::KPEnter;
        mapping[SDL_SCANCODE_KP_EQUALS]      = KeyCode::KPEqual;

        mapping[SDL_SCANCODE_LSHIFT]         = KeyCode::LeftShift;
        mapping[SDL_SCANCODE_LCTRL]          = KeyCode::LeftControl;
        mapping[SDL_SCANCODE_LALT]           = KeyCode::LeftAlt;
        mapping[SDL_SCANCODE_LGUI]           = KeyCode::LeftSuper;
        mapping[SDL_SCANCODE_RSHIFT]         = KeyCode::RightShift;
        mapping[SDL_SCANCODE_RCTRL]          = KeyCode::RightControl;
        mapping[SDL_SCANCODE_RALT]           = KeyCode::RightAlt;
        mapping[SDL_SCANCODE_RGUI]           = KeyCode::RightSuper;
        mapping[SDL_SCANCODE_MENU]           = KeyCode::Menu;

        return mapping;
    }();

    const std::array<MouseButton, 6> SDLToMouseButton = {
        MouseButton::None,    // SDL will never return this
        MouseButton::Button0, // Left
        MouseButton::Button1, // Right
        MouseButton::Button2, // Middle
        MouseButton::Button3, // X1
        MouseButton::Button4  // X2
    };

} // namespace Engine
