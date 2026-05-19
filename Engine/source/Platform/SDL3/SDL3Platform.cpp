#include "Platform/SDL3/SDL3Platform.h"
#include "Platform/SDL3/SDL3Window.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"

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
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                Ref<EventManager> em = Application::Get()->GetServiceLocator()->Get<EventManager>();
                if(em)
                {
                    em->QueueEvent(Hash32("Engine::Application::Quit"), CreateScope<Event>());
                }
            }
        }         
    }

    Scope<IWindow> SDL3Platform::CreateWindow(const WindowProperties& properties)
    {
        Scope<SDL3Window> window = CreateScope<SDL3Window>(properties);
        return window;
    }

} // namespace Engine
