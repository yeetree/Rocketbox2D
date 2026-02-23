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

        void DrawQuad(const Ref<ITexture>& texture, const Vec4& color, const Mat4& transform, uint32_t layer);

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

        IGraphicsDevice* m_GraphicsDevice;

        Mat4 m_ViewProjection;

        std::vector<RenderCommand> m_CommandQueue;
        std::map<uint64_t, Ref<IPipelineState>> m_PSOCache;
        std::vector<Ref<IBuffer>> m_FrameBuffers; // keep buffers alive during frame

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
        Ref<Material> m_QuadMaterial;
        
        // Default
        Ref<ITexture> m_DefaultTexture;
    };
}
#endif // ENGINE_RENDERER_RENDERER
