#ifndef ENGINE_INPUT_MOUSEBUTTON
#define ENGINE_INPUT_MOUSEBUTTON

#include <cstdint>

namespace Engine
{
    enum class MouseButton : uint8_t {
        // from glfw3.h (and then from https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/MouseCodes.h)
        Button0                = 0,
        Button1                = 1,
        Button2                = 2,
        Button3                = 3,
        Button4                = 4,
        Button5                = 5,
        Button6                = 6,
        Button7                = 7,

        Last             = Button7,
        Left             = Button0,
        Right            = Button1,
        Middle           = Button2
    };
} // namespace Engine


#endif // ENGINE_INPUT_MOUSEBUTTON
