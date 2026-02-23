#ifndef ENGINE_RENDERER_MATERIALINSTANCE
#define ENGINE_RENDERER_MATERIALINSTANCE

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Renderer/Material.h"

namespace Engine {
    class ENGINE_EXPORT MaterialInstance : public Material {
    public:
        MaterialInstance() = default;
        MaterialInstance(Ref<Material> parent);
        ~MaterialInstance() override = default;

        void Set(const std::string& name, const ShaderDataValue& value) override;
        void SetTexture(const std::string& name, Ref<ITexture> tex) override;

        std::map<uint32_t, UniformBlock>& GetUniformBlocks() override;
        std::map<uint32_t, Ref<ITexture>>& GetTextures() override;
        std::map<std::string, uint32_t>& GetTextureSlots() override;
        const ShaderLayout& GetShaderLayout() const override;
        
        Ref<IShader>  GetShader() override;
        Ref<Material> GetParent() const;

    private:
        Ref<Material> m_Parent;
        std::map<uint32_t, UniformBlock> m_UniformBlocks;
        std::map<uint32_t, Ref<ITexture>> m_Textures;
        std::map<std::string, uint32_t> m_TextureSlots;
    };
}

#endif // ENGINE_RENDERER_MATERIALINSTANCE
