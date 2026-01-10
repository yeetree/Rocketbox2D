#ifndef ENGINE_RENDERER_IBUFFER
#define ENGINE_RENDERER_IBUFFER


namespace Engine {
    enum class BufferType { Vertex, Index, Uniform, Storage };

    struct BufferDesc {
        size_t size;
        BufferType type;
        const void* data = nullptr;
        bool isDynamic = false;
    };

    class IBuffer {
    public:
        virtual ~IBuffer() = default;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        //virtual size_t GetSize() const = 0;
        //virtual BufferType GetType() const = 0;

        //virtual void UpdateData(const void* data, size_t size, size_t offset = 0) = 0;
    };

} // namespace Engine


#endif // ENGINE_RENDERER_IBUFFER
