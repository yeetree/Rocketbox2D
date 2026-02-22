#include "Engine/Renderer/RHI/UniformBlock.h"
#include "Engine/Renderer/RHI/ShaderLayout.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include "Engine/Math/Vector.h"
#include "Engine/Math/Matrix.h"
#include <variant>

namespace Engine
{
    UniformBlock::UniformBlock(const ShaderBinding& binding) : m_Binding(binding) {
        m_Data.resize(binding.totalSize);
        // Cache elements
        for (const auto& element : m_Binding.elements) {
            m_ElementCache[element.name] = element;
        }
    };

    void UniformBlock::Set(const std::string& name, const ShaderDataValue& value) {
        // Get offset
        auto it = m_ElementCache.find(name);
        if (it == m_ElementCache.end()) {
            LOG_CORE_WARN("Parameter {0} not found in uniform block {1}!", name, m_Binding.name);
            return;
        }

        ShaderElement element = it->second;

        if(GetValueType(value) != element.type) {
            LOG_CORE_WARN("Invalid value given for parameter {0} in uniform block {1}!", name, m_Binding.name);
        }

        // Get data in variant
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            ENGINE_CORE_ASSERT(sizeof(T) <= element.size, "Value size mismatch!");

            // Copy
            memcpy(m_Data.data() + element.offset, &arg, sizeof(T));
            m_Dirty = true;
        }, value);
    }

    bool UniformBlock::HasParameter(const std::string& name) {
        return m_ElementCache.find(name) != m_ElementCache.end();
    }

    const uint8_t* UniformBlock::GetData() const { return m_Data.data(); }
    size_t UniformBlock::GetSize() const { return m_Data.size(); }
    const ShaderBinding& UniformBlock::GetBinding() const { return m_Binding; }

    ShaderDataType UniformBlock::GetValueType(ShaderDataValue value) {
        return std::visit([](auto&& arg) -> ShaderDataType {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int>)   return ShaderDataType::Int;
            if constexpr (std::is_same_v<T, float>) return ShaderDataType::Float;
            if constexpr (std::is_same_v<T, Vec2>)  return ShaderDataType::Vec2;
            if constexpr (std::is_same_v<T, Vec3>)  return ShaderDataType::Vec3;
            if constexpr (std::is_same_v<T, Vec4>)  return ShaderDataType::Vec4;
            if constexpr (std::is_same_v<T, Mat4>)  return ShaderDataType::Mat4;
            
            return ShaderDataType::Float; // Shouldn't really happen
        }, value);
    }
} // namespace Engine