#ifndef ENGINE_CORE_ENGINESTRINGS
#define ENGINE_CORE_ENGINESTRINGS

#include <string>

// Put all hashed strings used in engine here to be registered on StringRegistry::Reset();

namespace Engine
{
    constexpr const char* EngineStrings[] = {
        // Events
        "Quit",
        "KeyPressed",
        "KeyReleased",
        "MouseMoved",
        "MouseScrolled",
        "MouseButtonPressed",
        "MouseButtonReleased"
        "WindowMoved",
        "WindowResized",
    };
} // namespace Engine



#endif // ENGINE_CORE_ENGINESTRINGS
