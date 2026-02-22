#ifndef RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER
#define RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER

#include <vulkan/vulkan_raii.hpp>
#include <map>

// fwd
class VulkanDevice;

// Information for binding a set
struct DescriptorSetKey {
    // binding info
    std::map<uint32_t, vk::DescriptorBufferInfo> buffers;
    std::map<uint32_t, vk::DescriptorImageInfo> textures;

    bool operator<(const DescriptorSetKey& other) const {
        if (buffers != other.buffers) return buffers < other.buffers;
        return textures < other.textures;
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
    
    std::map<DescriptorSetKey, vk::DescriptorSet> m_SetCache;
};

#endif // RENDERER_VULKAN_RHI_VULKANDESCRIPTORMANAGER
