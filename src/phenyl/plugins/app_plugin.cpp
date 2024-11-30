#include "app_plugin.h"
#include "phenyl/application.h"

using namespace phenyl::engine;

AppPlugin::AppPlugin (std::unique_ptr<ApplicationBase> app) : app{std::move(app)} {

}

std::string_view AppPlugin::getName () const noexcept {
    return "AppPlugin";
}

void AppPlugin::init (core::PhenylRuntime& runtime) {
    app->engineRuntime = &runtime;

    app->_init();
    runtime.addSystem<PostInit>("Application::postInit", app.get(), &ApplicationBase::postInit);
}

void AppPlugin::shutdown (phenyl::core::PhenylRuntime& runtime) {
    app->shutdown();
}


