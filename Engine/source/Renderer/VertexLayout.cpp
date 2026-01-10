#include "Engine/Renderer/VertexLayout.h"

namespace Engine
{
    static uint32_t GetSizeOfType(VertexElementType type)
    {
        switch (type)
        {
            case VertexElementType::Bool:   return 1;
            case VertexElementType::Int:    return 4;
            case VertexElementType::Float:  return 4;
            case VertexElementType::Vec2:   return 4 * 2;
            case VertexElementType::Vec3:   return 4 * 3;
            case VertexElementType::Vec4:   return 4 * 4;
            case VertexElementType::Mat2:   return 4 * 4;
            case VertexElementType::Mat3:   return 4 * 9;
            case VertexElementType::Mat4:   return 4 * 16;
        }
        return 0;
    }

    static uint32_t GetComponentsOfType(VertexElementType type)
    {
        switch (type)
        {
            case VertexElementType::Bool:   return 1;
            case VertexElementType::Int:    return 1;
            case VertexElementType::Float:  return 1;
            case VertexElementType::Vec2:   return 2;
            case VertexElementType::Vec3:   return 3;
            case VertexElementType::Vec4:   return 4;
            case VertexElementType::Mat2:   return 4;
            case VertexElementType::Mat3:   return 9;
            case VertexElementType::Mat4:   return 16;
        }
        return 0;
    }

    VertexElement::VertexElement(VertexElementType type, const std::string& name) : name(name), type(type), size(GetSizeOfType(type)), components(GetComponentsOfType(type)), offset(0), normalized(false) {}

    VertexLayout::VertexLayout(std::initializer_list<VertexElement> elements) : m_Elements(elements) {
        CalculateOffsetsAndStride();
    }

    void VertexLayout::CalculateOffsetsAndStride() {
        uint32_t offset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements) {
            element.offset = offset;
            offset += element.size;
            m_Stride += element.size;
        }
    }
} // namespace Engine
