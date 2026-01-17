#ifndef ENGINE_RENDERER_CAMERA
#define ENGINE_RENDERER_CAMERA

#include "engine_export.h"

#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {
    class ENGINE_EXPORT Camera {
    public:
        Camera(float verticalSize = 1.0);

        void SetPosition(const Vec2& position);
        void SetRotation(float rotation);
        void SetZoom(float zoom);

        void SetVerticalSize(float verticalSize);

        Vec2 GetPosition() const;
        float GetRotation() const;
        float GetZoom() const;

        float GetVerticalSize() const;

        void OnResize();

        const Mat4& GetViewProjectionMatrix();

    private:
        void RecalculateProjection();
        void RecalculateViewMatrix();
        Mat4 m_ProjectionMatrix;
        Mat4 m_ViewMatrix;
        Mat4 m_ViewProjectionMatrix;

        Vec2 m_Position = { 0.0f, 0.0f };
        float m_Rotation = 0.0f;
        float m_Zoom = 1.0f;

        float m_VerticalSize = 1.0;
        float m_AspectRatio = 1.0;

        bool m_ViewDirty = false;
        bool m_ProjectionDirty = false;
    };
}

#endif // ENGINE_RENDERER_CAMERA
