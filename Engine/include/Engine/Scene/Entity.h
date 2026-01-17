#ifndef ENGINE_SCENE_ENTITY
#define ENGINE_SCENE_ENTITY

#include <entt/entt.hpp>

namespace Engine
{
    class Scene;

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, entt::registry* registry) : m_EntityHandle(handle), m_Registry(registry) {}

        // Validation
        bool IsValid() const { 
            return m_EntityHandle != entt::null && m_Registry && m_Registry->valid(m_EntityHandle); 
        }

        // Component Management
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            return m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& GetComponent() {
            return m_Registry->get<T>(m_EntityHandle);
        }

        template<typename T>
        bool HasComponent() const {
            return m_Registry->all_of<T>(m_EntityHandle);
        }

        void Destroy() {
            m_Registry->destroy(m_EntityHandle);
            m_EntityHandle = entt::null;
        }

    private:
        entt::entity m_EntityHandle{ entt::null };
        entt::registry* m_Registry = nullptr;
    };
} // namespace Engine


#endif // ENGINE_SCENE_ENTITY
