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
        Material(std::shared_ptr<IShader> shader, const VertexLayout& layout);


        void SetBool(const std::string& name, bool value);
        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetVec2(const std::string& name, Vec2 value);
        void SetVec3(const std::string& name, Vec3 value);
        void SetVec4(const std::string& name, Vec4 value);
        void SetMat4(const std::string& name, Mat4 value);
        void SetTexture(const std::string& name, std::shared_ptr<ITexture> tex);

        void Apply(); 

    private:
        std::shared_ptr<IShader> m_Shader;
        std::shared_ptr<IPipelineState> m_PSO;
        std::map<std::string, std::shared_ptr<ITexture>> m_Textures;
        bool m_Dirty;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_MATERIAL
