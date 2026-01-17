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

    enum class ShaderStage { Vertex, Fragment, Compute, Geometry };

    struct ShaderDesc {
        // Map ShaderStage to string (expects source code)
        // Only compiles what ShaderStages are specified
        std::map<ShaderStage, std::string> sources; 
    };

    class IShader {
    public:
        IShader() {
            static uint32_t nextID = 0;
            m_ID = nextID++;
        }

        virtual ~IShader() = default;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        uint32_t GetID() const { return m_ID; }

        // Uniform functions
        virtual void SetBool(const std::string& name, bool value) = 0;
        virtual void SetInt(const std::string& name, int value) = 0;
        virtual void SetFloat(const std::string& name, float value) = 0;
        virtual void SetVec2(const std::string& name, Vec2 value) = 0;
        virtual void SetVec3(const std::string& name, Vec3 value) = 0;
        virtual void SetVec4(const std::string& name, Vec4 value) = 0;
        virtual void SetMat4(const std::string& name, Mat4 value) = 0;
    private:
        uint32_t m_ID;
    };
}

#endif // ENGINE_RENDERER_ISHADER
