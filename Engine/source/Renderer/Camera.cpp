#include "Engine/Core/Application.h"
#include "Engine/Renderer/Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Engine
{
    Camera::Camera(float verticalSize) : m_ViewDirty(true), m_ProjectionDirty(true) {
        OnResize();
        SetVerticalSize(verticalSize);
    }

    void Camera::SetPosition(const Vec2& position) {
        m_Position = position;
        m_ViewDirty = true;
    }

    void Camera::SetRotation(float rotation) {
        m_Rotation = rotation;
        m_ViewDirty = true;
    }

    void Camera::SetZoom(float zoom) {
        m_Zoom = std::max(0.01f, zoom);;
        m_ViewDirty = true;
    }

    void Camera::SetVerticalSize(float verticalSize) {
        m_VerticalSize = verticalSize;
        m_ProjectionDirty = true;
    }

    void Camera::OnResize() {
        m_AspectRatio = Application::Get().GetAspectRatio();
        m_ProjectionDirty = true;
    }

    Vec2 Camera::GetPosition() const { return m_Position; }
    float Camera::GetRotation() const { return m_Rotation; }
    float Camera::GetZoom() const { return m_Zoom; }
    float Camera::GetVerticalSize() const { return m_VerticalSize; }

    void Camera::RecalculateProjection() {
        float left   = -m_VerticalSize * m_AspectRatio * 0.5f;
        float right  =  m_VerticalSize * m_AspectRatio * 0.5f;
        float bottom = m_VerticalSize * 0.5f;
        float top    = -m_VerticalSize * 0.5f;

        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        //m_ProjectionMatrix[1][1] *= -1; // flip y

        m_ViewDirty = true;
    }

    const Mat4& Camera::GetViewProjectionMatrix() { 
        if(m_ProjectionDirty) {
            RecalculateProjection();
            m_ProjectionDirty = false;
        }
        if(m_ViewDirty) {
            RecalculateViewMatrix();
            m_ViewDirty = false;
        }
        return m_ViewProjectionMatrix;
    }


    void Camera::RecalculateViewMatrix() {
        Mat4 view = Mat4(1.0f);
        view = glm::scale(view, Vec3(m_Zoom, m_Zoom, 1.0f));
        view = glm::rotate(view, -m_Rotation, Vec3(0, 0, 1));
        view = glm::translate(view, Vec3(-m_Position.x, -m_Position.y, 0.0f));

        m_ViewMatrix = view;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
} // namespace Engine
