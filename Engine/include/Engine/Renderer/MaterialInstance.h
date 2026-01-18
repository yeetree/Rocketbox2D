#ifndef ENGINE_RENDERER_MATERIALINSTANCE
#define ENGINE_RENDERER_MATERIALINSTANCE

#include "engine_export.h"

#include "Engine/Renderer/Material.h"

namespace Engine {
    class MaterialInstance {
    public:
        MaterialInstance() = default;
        MaterialInstance(std::shared_ptr<Material> parent) : m_Parent(parent) {}

        // Overrides
        void Set(const std::string& name, ShaderUniformValue value) { m_UniformOverrides[name] = value; };
        void SetTexture(const std::string& name, std::shared_ptr<ITexture> texture) { m_TextureOverrides[name] = texture; };

        // Getters
        std::shared_ptr<Material> GetParent() const { return m_Parent; }
        const std::map<std::string, ShaderUniformValue>& GetUniformOverrides() const { return m_UniformOverrides; }
        const std::map<std::string, std::shared_ptr<ITexture>>& GetTextureOverrides() const { return m_TextureOverrides; }

    private:
        std::shared_ptr<Material> m_Parent;
        std::map<std::string, ShaderUniformValue> m_UniformOverrides;
        std::map<std::string, std::shared_ptr<ITexture>> m_TextureOverrides;
    };
}

#endif // ENGINE_RENDERER_MATERIALINSTANCE
