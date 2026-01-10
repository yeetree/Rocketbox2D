#ifndef ENGINE_RENDERER_ISHADER
#define ENGINE_RENDERER_ISHADER

#include "engine_export.h"

#include <map>
#include <string>

namespace Engine {
    enum class ShaderStage { Vertex, Fragment, Compute, Geometry };

    struct ShaderDesc {
        std::map<ShaderStage, std::string> sources; 
    };

    class IShader {
    public:
        virtual ~IShader() = default;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

       // virtual void SetBool(const std::string& name, bool value) = 0;
       // virtual void SetInt(const std::string& name, int value) = 0;
        //virtual void SetFloat(const std::string& name, float value) = 0;
    };
}

#endif // ENGINE_RENDERER_ISHADER
