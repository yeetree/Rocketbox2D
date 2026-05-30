#ifndef ENGINE_MATH_MATRIX
#define ENGINE_MATH_MATRIX

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine {

    // Wrap glm
    using Mat4 = glm::mat4;
}

#endif // ENGINE_MATH_MATRIX
