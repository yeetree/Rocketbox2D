#ifndef ENGINE_RHI_ISHADER
#define ENGINE_RHI_ISHADER

#include "engine_export.h"

#include <vector>
#include <string>
#include <map>
#include <cstdint>

namespace Engine
{
    enum class ENGINE_EXPORT ShaderStage { Vertex, Fragment };

    struct ENGINE_EXPORT ShaderModule {
        std::vector<uint32_t> byteCode;                 // Shader module blob
        std::map<ShaderStage, std::string> entryPoints; // Maps ShaderStage to string. String is used as an entry point
                                                        // name.
    };

    struct ENGINE_EXPORT ShaderDesc {
        std::vector<ShaderModule> modules;
    };

    class ENGINE_EXPORT IShader {
    private:
        uint32_t m_ID;

    protected:
        IShader() {
            static uint32_t nextID = 1;
            m_ID = nextID++;
        }

    public:
        virtual ~IShader() = default;
        
        uint32_t GetID() const { return m_ID; }
    };
} // namespace Engine


#endif // ENGINE_RHI_ISHADER
