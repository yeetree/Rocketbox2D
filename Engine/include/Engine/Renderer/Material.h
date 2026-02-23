#ifndef ENGINE_RENDERER_MATERIAL
#define ENGINE_RENDERER_MATERIAL

#include "engine_export.h"

#include <memory>
#include <string>

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IShader.h"
#include "Engine/Renderer/RHI/ITexture.h"
#include "Engine/Renderer/RHI/UniformBlock.h"

namespace Engine
{
    class ENGINE_EXPORT Material {
    public:
        Material() = default;
        Material(Ref<IShader> shader, ShaderLayout& layout);
        virtual ~Material() = default;

        virtual void Set(const std::string& name, const ShaderDataValue& value);
        virtual void SetTexture(const std::string& name, Ref<ITexture> tex);

        virtual std::map<uint32_t, UniformBlock>& GetUniformBlocks();
        virtual std::map<uint32_t, Ref<ITexture>>& GetTextures();
        virtual std::map<std::string, uint32_t>& GetTextureSlots();
        virtual const ShaderLayout& GetShaderLayout() const;
        virtual Ref<IShader> GetShader();

    private:
        Ref<IShader> m_Shader;
        ShaderLayout m_ShaderLayout;

        std::map<uint32_t, UniformBlock> m_UniformBlocks;
        std::map<std::string, uint32_t> m_TextureSlots;
        std::map<uint32_t, Ref<ITexture>> m_Textures;
        
    };
} // namespace Engine


#endif // ENGINE_RENDERER_MATERIAL
