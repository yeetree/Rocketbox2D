#ifndef ENGINE_CORE_RESOURCEMANAGER
#define ENGINE_CORE_RESOURCEMANAGER

#include "engine_export.h"

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
        void CreateMaterial(const std::string& identifier, std::shared_ptr<IShader> shader);

        void SetShader(const std::string& identifier, std::unique_ptr<IShader> shader);
        void SetTexture(const std::string& identifier, std::unique_ptr<ITexture> texture);
        void SetMesh(const std::string& identifier, std::unique_ptr<Mesh> mesh);
        void SetMaterial(const std::string& identifier, std::unique_ptr<Material> material);

        std::shared_ptr<IShader> GetShader(const std::string& identifier);
        std::shared_ptr<ITexture> GetTexture(const std::string& identifier);
        std::shared_ptr<Mesh> GetMesh(const std::string& identifier);
        std::shared_ptr<Material> GetMaterial(const std::string& identifier);
        
    private:
        // Graphics device for creating GPU resources
        IGraphicsDevice* m_GraphicsDevice;

        std::unordered_map<std::string, std::shared_ptr<IShader>> m_Shaders;
        std::unordered_map<std::string, std::shared_ptr<ITexture>> m_Textures;
        std::unordered_map<std::string, std::shared_ptr<Mesh>> m_Meshes;
        std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
    };
} // namespace Engine


#endif // ENGINE_CORE_RESOURCEMANAGER
