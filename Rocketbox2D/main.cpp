#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>
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
    const float movespeed = 2.0f;
    const float rotspeed = 4.0f;
    Vec2 camOffset = Vec2(0.0f, 0.0f);

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

        // If LSHIFT is down, offset camera so that it looks like it isn't moving
        if(Input::IsKeyDown(SDL_SCANCODE_LSHIFT)) {
            camOffset -= move;
        }

        GetComponent<TransformComponent>().position += move;
        GetComponent<TransformComponent>().rotation += rot;
        
        // Set camera position
        GetScene().GetEntity("Camera").GetComponent<TransformComponent>().position = GetComponent<TransformComponent>().position + camOffset;
    }
};


class Rocketbox2D : public Application {
public:
    Scene scene;
    Entity rocket;
    Entity ref;
    Entity camera;

    void OnStart() override {
        LOG_TRACE("Hello, Rocketbox2D!");

        scene.OnStart();

        GetResourceManager().LoadShader("basic", "./Assets/basic.vert", "./Assets/basic.frag");
        GetResourceManager().CreateMaterial("basicMaterial", GetResourceManager().GetShader("basic"));

        GetResourceManager().CreateMesh("shipMesh", shipVerts, sizeof(shipVerts), shipIndices, sizeof(shipIndices), sizeof(shipIndices), {VertexElement(VertexElementType::Vec2, "position")});

        rocket = scene.CreateEntity("Rocket");
        rocket.AddComponent<MeshComponent>(GetResourceManager().GetMesh("shipMesh"));
        rocket.AddComponent<MaterialComponent>(GetResourceManager().GetMaterial("basicMaterial"));
        rocket.GetComponent<MaterialComponent>().material->Set("u_Color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        rocket.GetComponent<TransformComponent>().scale = Vec2(1.0f, 1.0f);
        rocket.AddComponent<NativeScriptComponent>().Bind<RocketScript>();

        ref = scene.CreateEntity("RefEntity");
        ref.AddComponent<MeshComponent>(GetResourceManager().GetMesh("shipMesh"));
        ref.AddComponent<MaterialComponent>(GetResourceManager().GetMaterial("basicMaterial"));
        // TODO: Implement material instancing: This changes color of both entities
        ref.GetComponent<MaterialComponent>().material->Set("u_Color", Vec4(1.0f, 1.0f, 0.0f, 1.0f));
        ref.GetComponent<TransformComponent>().scale = Vec2(1.0f, 1.0f);

        camera = scene.CreateEntity("Camera");
        camera.AddComponent<CameraComponent>(10.0f);

    }

    void OnInput(SDL_Event event) override {
        scene.OnInput(event);
    }

    void OnUpdate(float dt) override {
        scene.OnUpdate(dt);
    }

    void OnRender() override {
        scene.OnRender();
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