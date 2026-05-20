#ifndef ENGINE_INPUT_INPUTBUTTON
#define ENGINE_INPUT_INPUTBUTTON

#include <cstdint>

namespace Engine
{
    enum class InputButton : uint8_t
    {
        None                = 0,
        Key                 = 1,
        MouseButton         = 2
    };

    constexpr uint8_t MaxInputButton = 2;
} // namespace Engine


#endif // ENGINE_INPUT_INPUTBUTTON
