#ifndef ENGINE_RENDERER_RENDERER
#define ENGINE_RENDERER_RENDERER

#include "engine_export.h"

#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"
#include "Engine/Renderer/MaterialInstance.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {

    class ENGINE_EXPORT Renderer {
    public:
        Renderer(IGraphicsDevice* device);
        Renderer(const Renderer&) = delete; // No copying
        ~Renderer() = default;

        void BeginScene(const Mat4& viewProjection);
        void EndScene();
        void Submit(Ref<Mesh> mesh, Ref<Material> material, const Mat4& transform, uint32_t layer);
        void Submit(Ref<Mesh> mesh, Ref<MaterialInstance> material, const Mat4& transform, uint32_t layer);

        void DrawQuad(const Ref<ITexture>& texture, const Vec4& color, const Mat4& transform, uint32_t layer);

    private:
        void Flush();
        Ref<IPipelineState> GetOrCreatePSO(Ref<Mesh> mesh, Ref<IShader> shader);

        struct RenderCommand {
            Ref<Mesh> mesh;
            Ref<IShader> shader;

            //std::map<std::string, ShaderUniformValue> uniforms; 
            std::map<std::string, Ref<ITexture>> textures;
            //std::map<std::string, ShaderUniformValue> uniformOverrides; 
            std::map<std::string, Ref<ITexture>> textureOverrides;

            Mat4 transform;
            
            // Top bits: Shader ID, Middle bits: Texture ID, Bottom bits: Mesh ID
            uint64_t sortKey; 
        };

        IGraphicsDevice* m_GraphicsDevice;

        Mat4 m_ViewProjection;
        std::vector<RenderCommand> m_CommandQueue;
        std::map<uint64_t, Ref<IPipelineState>> m_PSOCache;

        // UBO
        struct RendererUniformData {
            alignas(16) Engine::Mat4 viewProjection;
        };
        RendererUniformData uniformData;

        Ref<IUniformBuffer> m_UBO;

        // Quads
        Ref<Mesh> m_QuadMesh;
        Ref<Material> m_QuadMaterial;
        
        // Default
        Ref<ITexture> m_DefaultTexture;
    };
}
#endif // ENGINE_RENDERER_RENDERER
