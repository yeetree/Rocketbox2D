#ifndef ENGINE_SCENE_ENTITYSCRIPT
#define ENGINE_SCENE_ENTITYSCRIPT

#include "Engine/Scene/Entity.h"

namespace Engine {
    class EntityScript {
    public:
        virtual ~EntityScript() {}

        template<typename T>
        T& GetComponent() { return m_Entity.GetComponent<T>(); }

    protected:
        virtual void Start() {}
        virtual void Destroy() {}
        virtual void Update(float dt) {}

    private:
        Entity m_Entity;
        friend class Scene;
    };
}

#endif // ENGINE_SCENE_ENTITYSCRIPT
