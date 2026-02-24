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

        void DrawSprite(const Ref<ITexture>& texture, const Vec4& color, const Mat4& transform, uint32_t layer);

    private:
        void Flush();
        Ref<IPipelineState> GetOrCreatePSO(Ref<Mesh> mesh, Ref<Material> material);

        struct RenderCommand {
            Ref<Mesh> mesh;
            Ref<Material> material;

            Mat4 transform;

            std::vector<uint8_t> localData;
            
            uint64_t sortKey; 
        };

        struct SpriteUniform {
            alignas(16) Mat4 transform;
            alignas(16) Vec4 color;
        };

        struct SpriteCommand {
            Ref<ITexture> texture;
            SpriteUniform data;
            uint64_t layer;
        };

        IGraphicsDevice* m_GraphicsDevice;

        Mat4 m_ViewProjection;

        std::vector<RenderCommand> m_CommandQueue;
        std::vector<SpriteCommand> m_SpriteQueue;
        
        std::map<uint64_t, Ref<IPipelineState>> m_PSOCache;

        struct BufferPool {
            std::vector<Ref<IBuffer>> pool;
            uint32_t usedCount = 0;

            Ref<IBuffer> GetNext(IGraphicsDevice* device, size_t size) {
                if (usedCount < pool.size()) {
                    return pool[usedCount++];
                }
                // Pool is empty, grow it
                BufferDesc desc{ .size = size, .type = BufferType::Uniform, .isDynamic = true };
                Ref<IBuffer> buf = device->CreateBuffer(desc);
                pool.push_back(buf);
                usedCount++;
                return buf;
            }

            void Reset() { usedCount = 0; }
        };

        BufferPool m_BufferPool;
        UniformBlock m_GlobalData;
        Ref<IBuffer> m_GlobalBuffer;

        // Quads
        Ref<Mesh> m_QuadMesh;
        Ref<IPipelineState> m_SpritePipeline;
        
        // Default
        Ref<ITexture> m_DefaultTexture;
    };
}
#endif // ENGINE_RENDERER_RENDERER
