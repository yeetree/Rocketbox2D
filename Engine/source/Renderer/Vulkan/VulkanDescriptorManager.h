#ifndef RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER
#define RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER

#include <vulkan/vulkan_raii.hpp>
#include <map>
#include <unordered_map>

// fwd
class VulkanDevice;

// Information for binding a set
struct DescriptorSetKey {
    size_t hash = 0;

    std::map<uint32_t, vk::DescriptorBufferInfo> buffers;
    std::map<uint32_t, vk::DescriptorImageInfo> textures;

    bool operator==(const DescriptorSetKey& other) const {
        if (hash != other.hash) return false;
        return buffers == other.buffers && textures == other.textures;
    }
};

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(VulkanDevice& device);

    vk::DescriptorSet GetDescriptorSet(vk::DescriptorSetLayout layout, const DescriptorSetKey& key);

    void Reset();

private:
    VulkanDevice& m_Device;
    vk::raii::DescriptorPool m_Pool = nullptr;

    // hasher for unordered_map
    struct DescriptorSetHasher {
        size_t operator()(const DescriptorSetKey& key) const {
            return key.hash;
        }
    };
    
    std::unordered_map<DescriptorSetKey, vk::DescriptorSet, DescriptorSetHasher> m_SetCache;
};

#endif // RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER
