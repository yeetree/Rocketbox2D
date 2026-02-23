#ifndef ENGINE_RENDERER_RHI_SHADERLAYOUT
#define ENGINE_RENDERER_RHI_SHADERLAYOUT

#include "engine_export.h"

#include <vector>
#include <string>
#include <cstdint>
#include <variant>

#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"

namespace Engine {
    // Standard data types found in UBOs
    enum class ShaderDataType { Float, Vec2, Vec3, Vec4, Mat4, Int };
    using ShaderDataValue = std::variant<int, float, Vec2, Vec3, Vec4, Mat4>;

    enum class ShaderBindingType { UniformBuffer, Sampler };

    // fwd
    struct ShaderElement;

    struct ShaderBinding {
        std::string name = "";
        ShaderBindingType type; 
        uint32_t slot = 0;
        uint32_t binding = 0;
        uint32_t set = 0;
        uint32_t totalSize = 0;
        std::vector<ShaderElement> elements = {};

        // Constructor for ubps
        ShaderBinding() = default;
        ShaderBinding(ShaderBindingType type, const std::string& name, uint32_t slot = 0, uint32_t binding = 0, uint32_t set = 0, std::initializer_list<ShaderElement> elements = {})
            : type(type), name(name), slot(slot), binding(binding), set(set), elements(elements) {}
    };

    class ENGINE_EXPORT ShaderLayout {
    public:
        ShaderLayout(std::initializer_list<ShaderBinding> blocks = {});

        const std::vector<ShaderBinding>& GetBindings() const { return m_Bindings; }
        const ShaderBinding* GetBinding(const std::string& name) const;
        const ShaderBinding* GetBindingBySlot(uint32_t slot) const;
        uint64_t GetHash() const; 

    private:
        friend class ShaderElement;
        static uint32_t GetShaderDataTypeSize(ShaderDataType type);
        static uint32_t GetShaderDataTypeAlignment(ShaderDataType type);

        std::vector<ShaderBinding> m_Bindings;
    };

    struct ShaderElement {
        std::string name;
        ShaderDataType type;
        uint32_t size;
        uint32_t offset;

        ShaderElement() = default;
        ShaderElement(ShaderDataType type, const std::string& name)
            : name(name), type(type), size(ShaderLayout::GetShaderDataTypeSize(type)), offset(0) {}
    };
}

#endif // ENGINE_RENDERER_RHI_SHADERLAYOUT
