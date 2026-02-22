#include "Engine/Renderer/Material.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    Material::Material(Ref<IShader> shader, ShaderLayout& layout) : m_Shader(shader), m_Layout(layout) {
        // Init blocks and texture mappings
        for (const auto& binding : layout.GetBindings()) {
            switch(binding.type) {
                case ShaderBindingType::UniformBuffer: {
                    m_UniformBlocks.emplace(binding.name, UniformBlock(binding));
                    break;
                }
                case ShaderBindingType::Sampler: {
                    m_TextureSlots[binding.name] = binding.slot;
                    break;
                }
            }
        }
    }

    void Material::Set(const std::string& name, const ShaderDataValue& value) {
        for (auto& [blockName, block] : m_UniformBlocks) {
            if (block.HasParameter(name)) {
                block.Set(name, value);
                return;
            }
        }
        LOG_CORE_WARN("Parameter '{0}' not found in any uniform block in shader layout.", name);
    }

    void Material::SetTexture(const std::string& name, Ref<ITexture> tex) {
        if (m_TextureSlots.contains(name)) {
            uint32_t slot = m_TextureSlots[name];
            m_Textures[slot] = tex;
        } else {
            LOG_CORE_WARN("Texture '{0}' not found in shader layout.", name);
        }
    }

    std::map<std::string, UniformBlock>& Material::GetUniformBlocks() { return m_UniformBlocks; }
    std::map<uint32_t, Ref<ITexture>>& Material::GetTextures() { return m_Textures; };
    std::map<std::string, uint32_t>& Material::GetTextureSlots() { return m_TextureSlots; }
    const ShaderLayout& Material::GetLayout() const { return m_Layout; };

    Ref<IShader> Material::GetShader() { return m_Shader; };
} // namespace Engine
