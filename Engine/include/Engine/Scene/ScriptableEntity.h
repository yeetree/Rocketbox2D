#ifndef ENGINE_SCENE_SCRIPTABLEENTITY
#define ENGINE_SCENE_SCRIPTABLEENTITY

#include <SDL3/SDL.h>

#include "Engine/Scene/Entity.h"

namespace Engine {
    class ScriptableEntity {
    public:
        virtual ~ScriptableEntity() {}

        template<typename T>
        bool HasComponent() { return m_Entity.HasComponent<T>(); }

        template<typename T>
        T& GetComponent() { return m_Entity.GetComponent<T>(); }

        template<typename T>
        T& AddComponent() { return m_Entity.AddComponent<T>(); }

        Scene& GetScene() { return m_Entity.GetScene(); }

    protected:
        virtual void OnStart() {}
        virtual void OnDestroy() {}
        virtual void OnInput(SDL_Event event) {}
        virtual void OnUpdate(float ts) {}

    private:
        Entity m_Entity;
        friend class Scene;
    };
}

#endif // ENGINE_SCENE_SCRIPTABLEENTITY
