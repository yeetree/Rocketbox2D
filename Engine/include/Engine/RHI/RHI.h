#ifndef ENGINE_RHI_RHI
#define ENGINE_RHI_RHI

#include "Engine/RHI/VertexLayout.h"

#include "Engine/Core/Flags.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

// ============================================================================
// Forward declarations
// ============================================================================

namespace Engine
{
    class IWindow;
}

namespace Engine::RHI
{
    // ========================================================================
    // Enums
    // ========================================================================

    enum class GraphicsAPI       { Vulkan };
    enum class BufferType        { Vertex, Index, Uniform };
    enum class BufferUsage       { Static, Dynamic };
    enum class PixelFormat       { RGBA8 };
    enum class PresentMode       { Immediate, VSync, Mailbox };
    enum class ShaderStage       { Vertex, Fragment };
    enum class PrimitiveTopology { PointList, LineList, TriangleList };
    enum class PolygonMode       { Fill, Line, Point };
    enum class CullMode          { None, Back, Front };
    enum class FrontFace         { Clockwise, CounterClockwise };
    enum class UniformType       { UniformBuffer, Texture };

    // ========================================================================
    // Flags
    // ========================================================================

    enum class TextureUsage : uint32_t {
        None         = 0,
        Sampled      = 1 << 0,
        RenderTarget = 1 << 1,
    };
    using TextureUsageFlags = Flags<TextureUsage>;

    // ========================================================================
    // Structs
    // ========================================================================

    struct UniformBinding {
        uint32_t    binding;
        ShaderStage stage;
        UniformType type;
    };

    // =========================================================================
    // Handles
    // =========================================================================

    struct BufferHandle    { uint32_t id = 0; bool IsValid() const { return id != 0; } bool operator==(const BufferHandle&)    const = default; };
    struct TextureHandle   { uint32_t id = 0; bool IsValid() const { return id != 0; } bool operator==(const TextureHandle&)   const = default; };
    struct ShaderHandle    { uint32_t id = 0; bool IsValid() const { return id != 0; } bool operator==(const ShaderHandle&)    const = default; };
    struct PipelineHandle  { uint32_t id = 0; bool IsValid() const { return id != 0; } bool operator==(const PipelineHandle&)  const = default; };
    struct SwapChainHandle { uint32_t id = 0; bool IsValid() const { return id != 0; } bool operator==(const SwapChainHandle&) const = default; };

    // ========================================================================
    // Descs
    // ========================================================================

    struct BufferDesc {
        size_t      size  = 0;
        BufferType  type  = BufferType::Vertex;
        BufferUsage usage = BufferUsage::Static;
    };

    struct TextureDesc {
        uint32_t          width  = 0;
        uint32_t          height = 0;
        PixelFormat       format = PixelFormat::RGBA8;
        TextureUsageFlags usage  = TextureUsage::Sampled;
    };

    struct ShaderModule {
        std::vector<uint32_t> spirv;
        std::unordered_map<ShaderStage, std::string> entryPoints;
    };

    struct ShaderDesc {
        std::vector<ShaderModule> modules;
    };

    struct PipelineDesc {
        ShaderHandle                shader;
        VertexLayout                vertexLayout;
        std::vector<UniformBinding> uniformBindings;
        std::vector<PixelFormat>    colorAttachmentFormats;
        PrimitiveTopology           topology    = PrimitiveTopology::TriangleList;
        PolygonMode                 polygonMode = PolygonMode::Fill;
        CullMode                    cullMode    = CullMode::Back;
        FrontFace                   frontFace   = FrontFace::Clockwise;
        bool                        blending    = false;
    };

    struct SwapChainDesc {
        Engine::IWindow* window;
        PresentMode      presentation = PresentMode::VSync;
        PixelFormat      format       = PixelFormat::RGBA8;
    };
} // namespace Engine::RHI


#endif // ENGINE_RHI_RHI
