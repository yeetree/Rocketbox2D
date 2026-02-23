#include "Engine/Renderer/RHI/VertexLayout.h"
#include "Engine/Core/Log.h"

namespace Engine
{
    uint32_t VertexLayout::GetSizeOfType(VertexElementType type)
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

    VertexLayout::VertexLayout(std::initializer_list<VertexElement> elements) : m_Elements(elements) {
        CalculateOffsetsAndStride();
    }

    uint64_t VertexLayout::GetHash() const {
        // Hash layout
        uint64_t hash = 0;
        for (const auto& element : m_Elements) {
            hash ^= std::hash<int>{}((int)element.type) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
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
