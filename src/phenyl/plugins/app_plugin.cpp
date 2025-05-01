#include "app_plugin.h"
#include "phenyl/application.h"

using namespace phenyl::engine;

AppPlugin::AppPlugin (std::unique_ptr<ApplicationBase> app) : m_app{std::move(app)} {

}

std::string_view AppPlugin::getName () const noexcept {
    return "AppPlugin";
}

void AppPlugin::init (core::PhenylRuntime& runtime) {
    m_app->m_runtime = &runtime;

    m_app->_init();
    runtime.addSystem<PostInit>("Application::postInit", m_app.get(), &ApplicationBase::postInit);
}

void AppPlugin::shutdown (phenyl::core::PhenylRuntime& runtime) {
    m_app->shutdown();
}


