#include "Engine/Renderer/Material.h"

namespace Engine
{
    Material::Material(IGraphicsDevice *device, std::shared_ptr<IShader> shader, const VertexLayout& layout) : m_Shader(shader), m_Dirty(false) {
        // Create PSO
        PipelineDesc desc{};
        desc.shader = m_Shader.get();
        desc.layout = layout;

        // Temporary
        desc.enableBlending = true;

        m_PSO = device->CreatePipelineState(desc);
    }

    void Material::SetBool(const std::string& name, bool value) {
        m_Shader->Bind();
        m_Shader->SetBool(name, value);
    }

    void Material::SetInt(const std::string& name, int value) {
        m_Shader->Bind();
        m_Shader->SetInt(name, value);
    }

    void Material::SetFloat(const std::string& name, float value) {
        m_Shader->Bind();
        m_Shader->SetFloat(name, value);
    }

    void Material::SetVec2(const std::string& name, Vec2 value) {
        m_Shader->Bind();
        m_Shader->SetVec2(name, value);
    }

    void Material::SetVec3(const std::string& name, Vec3 value) {
        m_Shader->Bind();
        m_Shader->SetVec3(name, value);
    }

    void Material::SetVec4(const std::string& name, Vec4 value) {
        m_Shader->Bind();
        m_Shader->SetVec4(name, value);
    }

    void Material::SetMat4(const std::string& name, Mat4 value) {
        m_Shader->Bind();
        m_Shader->SetMat4(name, value);
    }

    void Material::SetTexture(const std::string& name, std::shared_ptr<ITexture> tex) {
        m_Textures[name] = tex;
        m_Dirty = true;
    }

    void Material::Apply() {
        m_PSO->Bind();
        if(m_Dirty) {
            int index = 0;
            for (const auto& [key, value] : m_Textures) {
                value->Bind(index);
                m_Shader->SetInt(key, index); // PSO uses shader
                index++;
            }
            m_Dirty = false;
        }
    }

} // namespace Engine
