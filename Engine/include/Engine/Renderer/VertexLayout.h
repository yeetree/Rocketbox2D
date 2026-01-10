#ifndef ENGINE_RENDERER_VERTEXLAYOUT
#define ENGINE_RENDERER_VERTEXLAYOUT

#include "engine_export.h"

#include <vector>
#include <string>

namespace Engine {
    enum class VertexElementType { Int, Float, Bool, Vec2, Vec3, Vec4, Mat2, Mat3, Mat4 };

    struct ENGINE_EXPORT VertexElement {
        std::string name;
        VertexElementType type;
        uint32_t components;
        uint32_t size;
        uint32_t offset;
        bool normalized;

        VertexElement(VertexElementType type, const std::string& name);
    };

    class ENGINE_EXPORT VertexLayout {
    public:
        VertexLayout(std::initializer_list<VertexElement> elements);

        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<VertexElement>& GetElements() const { return m_Elements; }

    private:
        void CalculateOffsetsAndStride();

        std::vector<VertexElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}

#endif // ENGINE_RENDERER_VERTEXLAYOUT
