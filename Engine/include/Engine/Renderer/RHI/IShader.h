#ifndef ENGINE_RENDERER_ISHADER
#define ENGINE_RENDERER_ISHADER

#include "engine_export.h"

#include <map>
#include <string>
#include <cstdint>
#include <variant>

#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {
    // What shader stage the current source is

    using ShaderUniformValue = std::variant<bool, int, float, Vec2, Vec3, Vec4, Mat4>;

    enum class ShaderStage { Vertex, Fragment, Geometry };

    struct ShaderModuleBlob {
        std::vector<char> byteCode;
        const char* entryPoint = "main";
    };

    struct ShaderDesc {
        // Map ShaderStage to string (expects source code)
        // Only compiles what ShaderStages are specified
        std::map<ShaderStage, ShaderModuleBlob> stages;
    };

    class ENGINE_EXPORT IShader {
    public:
        IShader() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

        virtual ~IShader() = default;
        

        uint32_t GetID() const { return m_ID; }

    private:
        uint32_t m_ID;
    };
}

#endif // ENGINE_RENDERER_ISHADER
