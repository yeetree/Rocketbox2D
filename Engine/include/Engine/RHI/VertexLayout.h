#ifndef ENGINE_RHI_VERTEXLAYOUT
#define ENGINE_RHI_VERTEXLAYOUT

#include "engine_export.h"

#include <vector>
#include <string>
#include <cstdint>

namespace Engine {
    // Type of vertex element
    enum class ENGINE_EXPORT VertexElementType { Int, Float, Vec2, Vec3, Vec4 };

    // Describes a vertex element
    class ENGINE_EXPORT VertexElement {
    private:
        friend class VertexLayout;

        std::string m_Name = "";
        VertexElementType m_Type;
        uint32_t m_Size = 0;
        uint32_t m_Offset = 0;
        bool m_Normalized = false;

        static uint32_t GetSizeOfType(VertexElementType type);
    public:
        VertexElement(VertexElementType type, const std::string& name, bool normalized = false);
    
        std::string GetName() const { return m_Name; }
        VertexElementType GetType() const { return m_Type; }
        uint32_t GetSize() const { return m_Size; }
        uint32_t GetOffset() const { return m_Offset; }
        bool IsNormalized() const { return m_Normalized; }
    };


    // VertexLayout is used to describe vertex attributes for a shader.
    class ENGINE_EXPORT VertexLayout {
    private:
        std::vector<VertexElement> m_Elements;
        uint32_t m_Stride = 0;
    
    public:
        VertexLayout(std::initializer_list<VertexElement> elements = {});

        uint32_t GetStride() const { return m_Stride; }
        const std::vector<VertexElement>& GetElements() const { return m_Elements; }
    };
}

#endif // ENGINE_RHI_VERTEXLAYOUT
