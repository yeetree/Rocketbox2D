#ifndef ENGINE_RENDERER_RHI_VERTEXLAYOUT
#define ENGINE_RENDERER_RHI_VERTEXLAYOUT

#include "engine_export.h"

#include <vector>
#include <string>
#include <cstdint>

namespace Engine {
    // Type of vertex element (We do not support passing matrices as vertex data)
    enum class VertexElementType { Int, Float, Vec2, Vec3, Vec4 };

    // Describes a vertex element
    struct ENGINE_EXPORT VertexElement {
        std::string name = "";
        VertexElementType type;
        uint32_t size = 0;      // Trust me, uint32_t makes sense here. (Vulkan is holding me at gunpoint)
        uint32_t offset = 0;
        bool normalized = false;

        VertexElement(VertexElementType type, const std::string& name);
    };

    // Creates a VertexLayout with initializer list of VertexElements, calculates stride, and updates size, components, and offset of each element
    class ENGINE_EXPORT VertexLayout {
    public:
        VertexLayout(std::initializer_list<VertexElement> elements = {});

        inline uint32_t GetStride() const { return m_Stride; }
        inline const std::vector<VertexElement>& GetElements() const { return m_Elements; }

        uint64_t GetHash() const;

    private:
        void CalculateOffsetsAndStride();

        std::vector<VertexElement> m_Elements;
        uint32_t m_Stride = 0;
    };
}

#endif // ENGINE_RENDERER_RHI_VERTEXLAYOUT
