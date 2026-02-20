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

    // What shader stage the current source is
    enum class ShaderStage { Vertex, Fragment, Geometry };

    struct ShaderModule {
        std::vector<uint32_t> byteCode;                 // Contains an entire shader module blob
        std::map<ShaderStage, std::string> entryPoints; // Maps shaderstage to string which is used as an entrypoint
                                                        // compiles only what stages are specified
    };

    struct ShaderDesc {
        std::vector<ShaderModule> modules;
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
