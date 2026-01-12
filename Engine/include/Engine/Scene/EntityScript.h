#ifndef ENGINE_SCENE_ENTITYSCRIPT
#define ENGINE_SCENE_ENTITYSCRIPT

#include <SDL3/SDL.h>
#include "Engine/Scene/Entity.h"

namespace Engine {
    class EntityScript {
    public:
        virtual ~EntityScript() {}

        template<typename T>
        T& GetComponent() { return m_Entity.GetComponent<T>(); }

    protected:
        virtual void OnStart() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnInput(SDL_Event event) {}

    private:
        Entity m_Entity;
        friend class Scene;
    };
}

#endif // ENGINE_SCENE_ENTITYSCRIPT
