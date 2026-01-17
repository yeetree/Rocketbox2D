#ifndef ENGINE_RENDERER_MATERIAL
#define ENGINE_RENDERER_MATERIAL

#include "engine_export.h"

#include <memory>
#include <string>

#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/IPipelineState.h"
#include "Engine/Renderer/RHI/IGraphicsDevice.h"

namespace Engine
{
    class ENGINE_EXPORT Material {
    public:
        Material() = default;
        Material(std::shared_ptr<IShader> shader);

        void Set(const std::string& name, ShaderUniformValue value) { m_Uniforms[name] = value; }
        void SetTexture(const std::string& name, std::shared_ptr<ITexture> tex) { m_Textures[name] = tex; }

        std::shared_ptr<IShader> GetShader() const { return m_Shader; }
        std::map<std::string, ShaderUniformValue> GetUniforms() const { return m_Uniforms; }
        std::map<std::string, std::shared_ptr<ITexture>> GetTextures() const { return m_Textures; }

    private:
        std::shared_ptr<IShader> m_Shader;
        
        std::map<std::string, ShaderUniformValue> m_Uniforms;
        std::map<std::string, std::shared_ptr<ITexture>> m_Textures;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_MATERIAL
