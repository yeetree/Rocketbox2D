#ifndef ENGINE_RENDERER_RENDERER2D
#define ENGINE_RENDERER_RENDERER2D

#include "engine_export.h"

#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {
    class ENGINE_EXPORT Renderer2D {
    public:
        Renderer2D(IGraphicsDevice* device);
        Renderer2D(const Renderer2D&) = delete; // No copying
        ~Renderer2D() = default;

        void BeginScene(const Mat4& viewProj);
        void DrawQuad(const std::shared_ptr<ITexture>& tex, const Vec2& pos, const Vec2& size, float rotation = 0, Vec4 tint = Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        void EndScene();

    private:
        IGraphicsDevice* m_GraphicsDevice;
        std::unique_ptr<IShader> m_QuadShader;
        std::unique_ptr<IBuffer> m_QuadVBO;
        std::unique_ptr<IBuffer> m_QuadEBO;
        std::unique_ptr<IPipelineState> m_QuadPipeline;
        Mat4 viewProj;
    };
}
#endif // ENGINE_RENDERER_RENDERER2D
