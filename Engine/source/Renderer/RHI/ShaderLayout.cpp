#include "Engine/Renderer/RHI/ShaderLayout.h"

namespace Engine
{
    ShaderLayout::ShaderLayout(std::initializer_list<ShaderBinding> bindings) : m_Bindings(bindings) {
        for (auto& block : m_Bindings) {
            if (block.type == ShaderBindingType::UniformBuffer) {
                uint32_t currentOffset = 0;
                for (auto& element : block.elements) {
                    uint32_t alignment = GetShaderDataTypeAlignment(element.type);
                    
                    // Apply padding to align with std140 rules
                    uint32_t padding = (alignment - (currentOffset % alignment)) % alignment;
                    currentOffset += padding;
                    
                    element.offset = currentOffset;
                    currentOffset += element.size;
                }
                // Align ubo to 16 / 256 bytes
                block.totalSize = (currentOffset + 15) & ~15; 
            }
        }
    }

    uint64_t ShaderLayout::GetHash() const {
        uint64_t hash = 0;
        for (const auto& binding : m_Bindings) {
            hash ^= std::hash<uint32_t>{}(binding.set) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<uint32_t>{}(binding.binding) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<int>{}(static_cast<int>(binding.type)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }

    // Get size of shader data type in bytes
    uint32_t ShaderLayout::GetShaderDataTypeSize(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float: return 4;
            case ShaderDataType::Vec2:  return 8;
            case ShaderDataType::Vec3:  return 12;
            case ShaderDataType::Vec4:  return 16;
            case ShaderDataType::Mat4:  return 64;
            case ShaderDataType::Int:   return 4;
        }
        return 0;
    }

    // maintains std140 alignment
    uint32_t ShaderLayout::GetShaderDataTypeAlignment(ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float: return 4;
            case ShaderDataType::Vec2:  return 8;
            case ShaderDataType::Vec3:  return 16;
            case ShaderDataType::Vec4:  return 16;
            case ShaderDataType::Mat4:  return 16;
            case ShaderDataType::Int:   return 4;
        }
        return 0;
    }
} // namespace Engine
