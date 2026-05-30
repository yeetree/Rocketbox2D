#include "Engine/RHI/VertexLayout.h"

namespace Engine::RHI
{
    uint32_t VertexElement::GetSizeOfType(VertexElementType type)
    {
        switch (type)
        {
            case VertexElementType::Int:    return 4;
            case VertexElementType::Float:  return 4;
            case VertexElementType::Vec2:   return 4 * 2;
            case VertexElementType::Vec3:   return 4 * 3;
            case VertexElementType::Vec4:   return 4 * 4;
        }
        return 0;
    }

    VertexElement::VertexElement(VertexElementType type, const std::string& name, bool normalized)
        : m_Type(type), m_Name(name), m_Normalized(normalized), m_Size(GetSizeOfType(type))
    {

    }

    VertexLayout::VertexLayout(std::initializer_list<VertexElement> elements)
        : m_Elements(elements)
    {
        uint32_t currentOffset = 0;
        m_Stride = 0;
        for (auto& element : m_Elements) {
            element.m_Offset = currentOffset;
            currentOffset += element.m_Size;
            m_Stride += element.m_Size;
        }
    }
} // namespace Engine::RHI
