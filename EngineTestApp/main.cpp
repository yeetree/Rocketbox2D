#include <iostream>
#include "Engine/Engine.h"
#include <SDL3/SDL_main.h>

std::vector<float> vertsData = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.0f,  0.5f
};

std::vector<unsigned int> indicesData = {
    0, 1, 2
};

using namespace Engine;

class EngineTestApp : public Engine {
public:
    std::unique_ptr<IPipelineState> pipeline;
    std::shared_ptr<IShader> shader;
    std::unique_ptr<IBuffer> verts;
    std::unique_ptr<IBuffer> indices;

    uint64_t ticks_prev;

    void Startup() override {
        GetResourceManager().LoadShader("basic", "Assets/basic.vert", "Assets/basic.frag");
        shader = GetResourceManager().GetShader("basic");

        verts = GetGraphicsDevice().CreateBuffer(BufferDesc{
            vertsData.size() * sizeof(float),
            BufferType::Vertex,
            vertsData.data()
        });

        indices = GetGraphicsDevice().CreateBuffer(BufferDesc{
            indicesData.size() * sizeof(unsigned int),
            BufferType::Index,
            indicesData.data()
        });

        pipeline = GetGraphicsDevice().CreatePipelineState(PipelineDesc{
            shader.get(),
            VertexLayout({VertexElement(VertexElementType::Vec2, "position")}),
        });

        ticks_prev = SDL_GetTicks();
    }

    void Input(SDL_Event event) override {

    }

    void Update() override {
        uint64_t ticks_now = SDL_GetTicks();
        //std::cout << "FPS: " << 1000.0 / (ticks_now - ticks_prev) << std::endl;
        ticks_prev = ticks_now;
    }

    void Render() override {
        pipeline->Bind();
        verts->Bind();
        indices->Bind();
        pipeline->ApplyVertexLayout();
        GetGraphicsDevice().SubmitDraw(indicesData.size());
    }

    void Cleanup() override {

    }
};

int main(int argc, char **argv) {
    EngineTestApp app;
    app.Init(800, 600, "EngineTestApp", SDL_WINDOW_RESIZABLE);
    app.Run();
    return 0;
}