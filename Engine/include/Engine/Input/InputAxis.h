#ifndef ENGINE_INPUT_INPUTAXIS
#define ENGINE_INPUT_INPUTAXIS

#include <cstdint>

namespace Engine
{
    enum class InputAxis : uint8_t
    {
        None                = 0,
        MouseX              = 1,
        MouseY              = 2,
        ScrollX             = 3,
        ScrollY             = 4
    };

    constexpr uint8_t MaxInputAxis = 4;
} // namespace Engine


#endif // ENGINE_INPUT_INPUTAXIS
