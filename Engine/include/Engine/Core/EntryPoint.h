#ifndef ENGINE_CORE_ENTRYPOINT
#define ENGINE_CORE_ENTRYPOINT

// from https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/EntryPoint.h

#include <SDL3/SDL_main.h>

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Application.h"

namespace Engine
{
    // To be defined by app
    extern int EntryPoint(int argc, char** argv);

} // namespace Engine

int main(int argc, char** argv)
{
    try {
        return Engine::EntryPoint(argc, argv);
    }
	catch(const std::exception& e) {
        LOG_CORE_CRITICAL(e.what());
        return EXIT_FAILURE;
    }
}

#endif // ENGINE_CORE_ENTRYPOINT
