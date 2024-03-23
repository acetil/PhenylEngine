#include "app_plugin.h"

using namespace phenyl::engine;

AppPlugin::AppPlugin (Application* app) : app{app} {}

std::string_view AppPlugin::getName () const noexcept {
    return "AppPlugin";
}

void AppPlugin::init (runtime::PhenylRuntime& runtime) {
    app->engineRuntime = &runtime;

    app->init();
}

void AppPlugin::update (runtime::PhenylRuntime& runtime, double deltaTime) {
    app->update(deltaTime);
}

void AppPlugin::fixedUpdate (runtime::PhenylRuntime& runtime, double deltaTime) {
    app->fixedUpdate(deltaTime);
}

void AppPlugin::shutdown (phenyl::runtime::PhenylRuntime& runtime) {
    app->shutdown();
}


