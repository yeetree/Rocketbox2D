#ifndef ENGINE_RENDERER_RHI_ISHADER
#define ENGINE_RENDERER_RHI_ISHADER

#include "engine_export.h"

#include <map>
#include <string>
#include <cstdint>
#include <variant>

#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {

    using ShaderUniformValue = std::variant<int, float, Vec2, Vec3, Vec4, Mat4>;

    // What shader stage the current source is
    enum class ShaderStage { Vertex, Fragment, Geometry };

    struct ShaderModuleBlob {
        std::vector<char> byteCode;
        std::string entryPoint = "main";
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

#endif // ENGINE_RENDERER_RHI_ISHADER
