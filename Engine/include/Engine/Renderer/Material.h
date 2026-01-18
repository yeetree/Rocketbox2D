#ifndef ENGINE_RENDERER_MATERIAL
#define ENGINE_RENDERER_MATERIAL

#include <memory>
#include <string>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/IPipelineState.h"
#include "Engine/Renderer/RHI/IGraphicsDevice.h"

namespace Engine
{
    class Material {
    public:
        Material() = default;
        Material(Ref<IShader> shader) : m_Shader(shader) {};

        void Set(const std::string& name, ShaderUniformValue value) { m_Uniforms[name] = value; }
        void SetTexture(const std::string& name, Ref<ITexture> tex) { m_Textures[name] = tex; }

        Ref<IShader> GetShader() const { return m_Shader; }
        std::map<std::string, ShaderUniformValue> GetUniforms() const { return m_Uniforms; }
        std::map<std::string, Ref<ITexture>> GetTextures() const { return m_Textures; }

    private:
        Ref<IShader> m_Shader;
        
        std::map<std::string, ShaderUniformValue> m_Uniforms;
        std::map<std::string, Ref<ITexture>> m_Textures;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_MATERIAL
