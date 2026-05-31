#ifndef ENGINE_RHI_RHI
#define ENGINE_RHI_RHI

#include "Engine/RHI/VertexLayout.h"

#include "Engine/Core/Flags.h"
#include "Engine/Core/Handle.h"
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
    enum class PixelFormat       { RGBA8, Depth32, Depth24Stencil8 };
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
        DepthStencil = 1 << 2
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

    struct BufferTag {};
    struct TextureTag {};
    struct ShaderTag {};
    struct PipelineTag {};
    struct SwapChainTag {};

    using BufferHandle    = Handle<BufferTag>;
    using TextureHandle   = Handle<TextureTag>;
    using ShaderHandle    = Handle<ShaderTag>;
    using PipelineHandle  = Handle<PipelineTag>;
    using SwapChainHandle = Handle<SwapChainTag>;

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
        bool                        depthTest   = false;
        bool                        depthWrite  = true;
        PixelFormat                 depthFormat = PixelFormat::Depth32;
    };

    struct SwapChainDesc {
        Engine::IWindow* window;
        PresentMode      presentation = PresentMode::VSync;
        PixelFormat      format       = PixelFormat::RGBA8;
    };
} // namespace Engine::RHI


#endif // ENGINE_RHI_RHI
