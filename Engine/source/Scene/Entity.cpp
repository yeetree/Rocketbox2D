#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
    entt::entity Entity::GetHandle() const {
        return m_EntityHandle;
    }

    void Entity::RemoveScript() {
        return m_Scene->RemoveScript(*this);
    }
} // namespace Engine
