#ifndef ENGINE_RENDERER_RHI_UNIFORMBLOCK
#define ENGINE_RENDERER_RHI_UNIFORMBLOCK

#include "engine_export.h"

#include "Engine/Renderer/RHI/ShaderLayout.h"
#include "Engine/Core/Base.h"

#include <vector>
#include <cstdint>

namespace Engine
{
    // fwd
    class IGraphicsDevice;
    class IUniformBuffer;

    class ENGINE_EXPORT UniformBlock {
    public:
        UniformBlock() = default;
        UniformBlock(const ShaderBinding& binding);

        void Set(const std::string& name, const ShaderDataValue& value);
        bool HasParameter(const std::string& name);

        const uint8_t* GetData() const;
        size_t GetSize() const;
        const ShaderBinding& GetBinding() const;
        Ref<IUniformBuffer> GetUniformBuffer();

        void Upload(IGraphicsDevice* device);

    private:
        static ShaderDataType GetValueType(ShaderDataValue value);

        ShaderBinding m_Binding;
        std::vector<uint8_t> m_Data;
        std::unordered_map<std::string, ShaderElement> m_ElementCache;
        Ref<IUniformBuffer> m_UniformBuffer;

        bool m_Dirty = true;
    };
} // namespace Engine


#endif // ENGINE_RENDERER_RHI_UNIFORMBLOCK
