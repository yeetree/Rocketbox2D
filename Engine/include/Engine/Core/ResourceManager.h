#ifndef ENGINE_CORE_RESOURCEMANAGER
#define ENGINE_CORE_RESOURCEMANAGER

#include "engine_export.h"

#include "Engine/Core/Base.h"
#include "Engine/Renderer/RHI/IGraphicsDevice.h"
#include "Engine/Renderer/Mesh.h"
#include "Engine/Renderer/Material.h"

namespace Engine
{
    class ENGINE_EXPORT ResourceManager {
    public:
        ResourceManager(IGraphicsDevice* device) : m_GraphicsDevice(device) {}
        ResourceManager(const ResourceManager&) = delete; // No copying
        ~ResourceManager() = default;
        
        void LoadShader(const std::string& identifier, const std::string& vertPath, const std::string& fragPath);
        void LoadTexture(const std::string& identifier, const std::string& texturePath);

        void CreateMesh(const std::string& identifier, const void* vertices, uint32_t vSize, const void* indices, uint32_t iSize, uint32_t indexCount, const VertexLayout& layout, bool vDynamic = false, bool iDynamic = true);
        void CreateMaterial(const std::string& identifier, Ref<IShader> shader);

        void SetShader(const std::string& identifier, Scope<IShader> shader);
        void SetTexture(const std::string& identifier, Scope<ITexture> texture);
        void SetMesh(const std::string& identifier, Scope<Mesh> mesh);
        void SetMaterial(const std::string& identifier, Scope<Material> material);

        Ref<IShader> GetShader(const std::string& identifier);
        Ref<ITexture> GetTexture(const std::string& identifier);
        Ref<Mesh> GetMesh(const std::string& identifier);
        Ref<Material> GetMaterial(const std::string& identifier);
        
    private:
        // Graphics device for creating GPU resources
        IGraphicsDevice* m_GraphicsDevice;

        std::unordered_map<std::string, Ref<IShader>> m_Shaders;
        std::unordered_map<std::string, Ref<ITexture>> m_Textures;
        std::unordered_map<std::string, Ref<Mesh>> m_Meshes;
        std::unordered_map<std::string, Ref<Material>> m_Materials;
    };
} // namespace Engine


#endif // ENGINE_CORE_RESOURCEMANAGER
