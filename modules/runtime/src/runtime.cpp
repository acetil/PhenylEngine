#include "logging/logging.h"

#include "runtime/plugin.h"
#include "runtime/runtime.h"

using namespace phenyl::runtime;

static phenyl::Logger LOGGER{"RUNTIME", phenyl::PHENYL_LOGGER};

PhenylRuntime::PhenylRuntime (phenyl::component::ComponentManager&& compManager) : compManager{std::move(compManager)} {
    PHENYL_LOGI(LOGGER, "Initialised Phenyl runtime");
}

PhenylRuntime::~PhenylRuntime () = default;

void PhenylRuntime::registerResource (std::size_t typeIndex, IResource* resource) {
    PHENYL_ASSERT_MSG(!resources.contains(typeIndex), "Attempted to add resource \"{}\" but has already been added!", resource->getName());

    PHENYL_LOGI(LOGGER, "Registered resource \"{}\"", resource->getName());
    resources.emplace(typeIndex, resource);
}

void PhenylRuntime::registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin) {
    PHENYL_ASSERT(!plugins.contains(typeIndex));

    auto& pluginRef = *plugin;

    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", plugin->getName());
    plugins.emplace(typeIndex, std::move(plugin));

    pluginRef.init(*this);
}

void PhenylRuntime::pluginFrameBegin () {
    PHENYL_LOGD(LOGGER, "Running plugin frameBegin()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running frameBegin() for {}", plugin->getName());
        plugin->frameBegin(*this);
    }
}

void PhenylRuntime::pluginUpdate (double deltaTime) {
    PHENYL_LOGD(LOGGER, "Running plugin update()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running update() for {}", plugin->getName());
        plugin->update(*this, deltaTime);
    }
}

void PhenylRuntime::pluginRender (double deltaTime) {
    PHENYL_LOGD(LOGGER, "Running plugin render()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running render() for {}", plugin->getName());
        plugin->render(*this);
    }
}

void PhenylRuntime::pluginFixedUpdate (double deltaTime) {
    PHENYL_LOGD(LOGGER, "Running plugin fixedUpdate()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running fixedUpdate() for {}", plugin->getName());
        plugin->fixedUpdate(*this, deltaTime);
    }
}

void PhenylRuntime::pluginPhysicsUpdate (double deltaTime) {
    PHENYL_LOGD(LOGGER, "Running plugin physicsUpdate()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running physicsUpdate() for {}", plugin->getName());
        plugin->physicsUpdate(*this, deltaTime);
    }
}