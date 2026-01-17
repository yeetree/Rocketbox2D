#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using namespace Engine;

const float shipVerts[] = {
    0.0f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f
};

const unsigned int shipIndices[] {
    0, 1, 2
};

class RocketScript : public ScriptableEntity {
    const float movespeed = 200.0f;
    const float rotspeed = 4.0f;

    void OnUpdate(float dt) override {
        Vec2 move = Vec2(0.0f, 0.0f);
        float rot = 0.0f;

        if(Input::IsKeyDown(SDL_SCANCODE_A)) {
            move.x -= movespeed * dt;
        }
        if(Input::IsKeyDown(SDL_SCANCODE_D)) {
            move.x += movespeed * dt;
        }
        if(Input::IsKeyDown(SDL_SCANCODE_W)) {
            move.y += movespeed * dt;
        }
        if(Input::IsKeyDown(SDL_SCANCODE_S)) {
            move.y -= movespeed * dt;
        }
        if(Input::IsKeyDown(SDL_SCANCODE_Q)) {
            rot += rotspeed * dt;
        }
        if(Input::IsKeyDown(SDL_SCANCODE_E)) {
            rot -= rotspeed * dt;
        }

        GetComponent<TransformComponent>().position += move;
        GetComponent<TransformComponent>().rotation += rot;
    }
};


class Rocketbox2D : public Application {
public:
    Mat4 viewproj;
    Scene scene;
    Entity rocket;

    void OnStart() override {
        LOG_TRACE("Hello, Rocketbox2D!");

        float w = (float)GetWindowWidth();
        float h = (float)GetWindowHeight();

        GetResourceManager().LoadShader("basic", "./Assets/basic.vert", "./Assets/basic.frag");
        GetResourceManager().CreateMaterial("basicMaterial", GetResourceManager().GetShader("basic"));

        GetResourceManager().CreateMesh("shipMesh", shipVerts, sizeof(shipVerts), shipIndices, sizeof(shipIndices), sizeof(shipIndices), {VertexElement(VertexElementType::Vec2, "position")});

        viewproj = glm::ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f);

        rocket = scene.CreateEntity();
        rocket.AddComponent<MeshComponent>(GetResourceManager().GetMesh("shipMesh"));
        rocket.AddComponent<MaterialComponent>(GetResourceManager().GetMaterial("basicMaterial"));
        rocket.GetComponent<MaterialComponent>().material->Set("u_Color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        rocket.GetComponent<TransformComponent>().position = Vec2(100.0f, 100.0f);
        rocket.GetComponent<TransformComponent>().scale = Vec2(100.0f, 100.0f);
        rocket.AddComponent<NativeScriptComponent>().Bind<RocketScript>();
    }

    void OnInput(SDL_Event event) override {
        switch(event.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                viewproj = glm::ortho(0.0f, (float)GetWindowWidth(), 0.0f, (float)GetWindowHeight(), -1.0f, 1.0f);
                break;
        }
        scene.OnInput(event);
    }

    void OnUpdate(float dt) override {
        scene.OnUpdate(dt);
    }

    void OnRender() override {
        scene.OnRender(viewproj);
    }

    void OnDestroy() override {

    }
};

int main(int argc, char **argv) {
    Rocketbox2D app;
    app.Init(800, 600, "Rocketbox2D", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}