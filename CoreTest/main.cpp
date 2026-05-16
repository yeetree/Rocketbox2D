#include <iostream>
#include "Engine/Engine.h"
#include <cmath>

using namespace Engine;

class NumberResource : public Resource
{
private:
    float value;

public:
    NumberResource(float x) : value(x) {};

    float Get() const { return value; };
    void Set(float x) { value = x; }
};

struct NumberResourceCreateInfo : public ResourceInfo {
    float value;
    NumberResourceCreateInfo(float x) : value(x) {};
};

class NumberResourceLoader : public IResourceLoader
{
public:
    NumberResourceLoader() {};

    Scope<Resource> Load(const ResourceInfo& info) override
    {
        return nullptr;
    }

    Scope<Resource> Create(const ResourceInfo& info) override
    {
        const NumberResourceCreateInfo& numConfig = static_cast<const NumberResourceCreateInfo&>(info);
        return CreateScope<NumberResource>(numConfig.value);
    }
};

class EngineTestApp : public Application {
public:
    void OnStart() override {
        GetServiceLocator().Get<ResourceManager>()->RegisterLoader<NumberResource>(CreateScope<NumberResourceLoader>());
        GetServiceLocator().Get<ResourceManager>()->Create<NumberResource>("n1", NumberResourceCreateInfo(1));
        GetServiceLocator().Get<ResourceManager>()->Create<NumberResource>("n2", NumberResourceCreateInfo(2));
        GetServiceLocator().Get<ResourceManager>()->Get<NumberResource>("n1")->Set(67);

        Ref<NumberResource> n1 = GetServiceLocator().Get<ResourceManager>()->Get<NumberResource>("n1");
        Ref<NumberResource> n2 = GetServiceLocator().Get<ResourceManager>()->Get<NumberResource>("n2");

        LOG_INFO("TEST: {}, {}", n1->Get(), n2->Get());
    }

    //void OnEvent(Event& event) override {
    //
    //}

    void OnUpdate(float dt) override {

    }

    void OnRender() override {

    }

    void OnDestroy() override {

    }
};

int Engine::EntryPoint(int argc, char **argv) {
    EngineTestApp app;
    //WindowProperties props = {
    //    .title = "CoreTest",
    //    .width = 800,
    //    .height = 600,
    //    .api = GraphicsAPI::Vulkan,
    //    .resizeable = true,
    //};
    app.Init(/*props*/);
    app.Run();
    return 0;
}