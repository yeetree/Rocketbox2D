#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    MaterialInstance::MaterialInstance(Ref<Material> parent) : m_Parent(parent) {
        if(parent == nullptr) {
            LOG_CORE_ERROR("Material instance parent cannot be null!");
            return;
        }

        // Copy parent data
        m_UniformBlocks = parent->GetUniformBlocks();
        m_Textures = parent->GetTextures();
        m_TextureSlots = parent->GetTextureSlots();
    }

    void MaterialInstance::Set(const std::string& name, const ShaderDataValue& value) {
        for (auto& [blockName, block] : m_UniformBlocks) {
            if (block.HasParameter(name)) {
                block.Set(name, value);
                return;
            }
        }
        LOG_CORE_WARN("Parameter \"{0}\" not found in any uniform block in shader layout.", name);
    }

    void MaterialInstance::SetTexture(const std::string& name, Ref<ITexture> tex) {
        // Get the mapping from the parent
        const auto& slots = m_Parent->GetTextureSlots();
        
        auto it = slots.find(name);
        if (it != slots.end()) {
            uint32_t slot = it->second;
            m_Textures[slot] = tex;
        } else {
            LOG_CORE_WARN("Texture \"{0}\" not found in shader layout.", name);
        }
    }

    std::map<uint32_t, UniformBlock>& MaterialInstance::GetUniformBlocks() {
        return m_UniformBlocks;
    }

    std::map<uint32_t, Ref<ITexture>>& MaterialInstance::GetTextures() {
        return m_Textures;
    }

    std::map<std::string, uint32_t>& MaterialInstance::GetTextureSlots() {
        return m_TextureSlots;
    }

    const ShaderLayout& MaterialInstance::GetShaderLayout() const { return m_Parent->GetShaderLayout(); }

    Ref<IShader>  MaterialInstance::GetShader() { return m_Parent->GetShader(); }
    Ref<Material> MaterialInstance::GetParent() const { return m_Parent; }
} // namespace Engine

