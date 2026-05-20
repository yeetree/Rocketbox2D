#ifndef ENGINE_INPUT_MOUSEBUTTON
#define ENGINE_INPUT_MOUSEBUTTON

#include <cstdint>

namespace Engine
{
    enum class MouseButton : uint8_t
    {
        None                   = 0,

        Left                   = 1,
        Right                  = 2,
        Middle                 = 3
    };

    constexpr uint16_t MaxMouseButton = 3;
} // namespace Engine


#endif // ENGINE_INPUT_MOUSEBUTTON
