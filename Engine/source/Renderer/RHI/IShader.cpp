#include "Engine/Renderer/RHI/IShader.h"

namespace Engine
{
    void IShader::Set(const std::string& name, ShaderUniformValue value) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) SetBool(name, arg);
            else if constexpr (std::is_same_v<T, int>) SetInt(name, arg);
            else if constexpr (std::is_same_v<T, float>) SetFloat(name, arg);
            else if constexpr (std::is_same_v<T, Vec2>) SetVec2(name, arg);
            else if constexpr (std::is_same_v<T, Vec3>) SetVec3(name, arg);
            else if constexpr (std::is_same_v<T, Vec4>) SetVec4(name, arg);
            else if constexpr (std::is_same_v<T, Mat4>) SetMat4(name, arg);
        }, value);
    }
} // namespace Engine
