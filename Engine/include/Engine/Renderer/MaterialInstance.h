#ifndef ENGINE_RENDERER_MATERIALINSTANCE
#define ENGINE_RENDERER_MATERIALINSTANCE

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Renderer/Material.h"

namespace Engine {
    class MaterialInstance {
    public:
        MaterialInstance() = default;
        MaterialInstance(Ref<Material> parent) : m_Parent(parent) {}

        // Overrides
        //void Set(const std::string& name, ShaderUniformValue value) { m_UniformOverrides[name] = value; };
        //void SetTexture(const std::string& name, Ref<ITexture> texture) { m_TextureOverrides[name] = texture; };

        // Getters
        Ref<Material> GetParent() const { return m_Parent; }
        //const std::map<std::string, ShaderUniformValue>& GetUniformOverrides() const { return m_UniformOverrides; }
        //const std::map<std::string, Ref<ITexture>>& GetTextureOverrides() const { return m_TextureOverrides; }

    private:
        Ref<Material> m_Parent;
        //std::map<std::string, ShaderUniformValue> m_UniformOverrides;
        //std::map<std::string, Ref<ITexture>> m_TextureOverrides;
    };
}

#endif // ENGINE_RENDERER_MATERIALINSTANCE
