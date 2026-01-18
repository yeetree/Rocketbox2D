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
        void Submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const Mat4& transform);
        void Submit(std::shared_ptr<Mesh> mesh, std::shared_ptr<MaterialInstance> material, const Mat4& transform);

        void DrawQuad(const std::shared_ptr<ITexture>& texture, const Vec4& color, const Mat4& transform);

    private:
        void Flush();
        std::shared_ptr<IPipelineState> GetOrCreatePSO(std::shared_ptr<Mesh> mesh, std::shared_ptr<IShader> shader);

        struct RenderCommand {
            std::shared_ptr<Mesh> mesh;
            std::shared_ptr<IShader> shader;

            std::map<std::string, ShaderUniformValue> uniforms; 
            std::map<std::string, std::shared_ptr<ITexture>> textures;
            std::map<std::string, ShaderUniformValue> uniformOverrides; 
            std::map<std::string, std::shared_ptr<ITexture>> textureOverrides;

            Mat4 transform;
            
            // Top bits: Shader ID, Middle bits: Texture ID, Bottom bits: Mesh ID
            uint64_t sortKey; 
        };

        IGraphicsDevice* m_GraphicsDevice;

        Mat4 m_ViewProjection;
        std::vector<RenderCommand> m_CommandQueue;
        std::map<uint64_t, std::shared_ptr<IPipelineState>> m_PSOCache;

        // Quads
        std::shared_ptr<Mesh> m_QuadMesh;
        std::shared_ptr<Material> m_QuadMaterial;
    };
}
#endif // ENGINE_RENDERER_RENDERER
