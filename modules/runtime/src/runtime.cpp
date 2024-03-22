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

    resources.emplace(typeIndex, resource);
    PHENYL_LOGI(LOGGER, "Registered resource \"{}\"", resource->getName());
}

void PhenylRuntime::registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin) {
    PHENYL_DASSERT(!plugins.contains(typeIndex));
    PHENYL_TRACE(LOGGER, "Starting registration of plugin \"{}\"", plugin->getName());

    auto& pluginRef = *plugin;

    plugins.emplace(typeIndex, std::move(plugin));

    pluginRef.init(*this);
    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", pluginRef.getName());
}

void PhenylRuntime::registerPlugin (std::size_t typeIndex, IInitPlugin& plugin) {
    PHENYL_DASSERT(!initPlugins.contains(typeIndex));
    PHENYL_TRACE(LOGGER, "Starting registration of init plugin \"{}\"", plugin.getName());

    initPlugins.emplace(typeIndex);

    plugin.init(*this);
    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", plugin.getName());
}


void PhenylRuntime::pluginPostInit () {
    PHENYL_TRACE(LOGGER, "Running plugin postInit()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running postInit() for {}", plugin->getName());
        plugin->postInit(*this);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin postInit()");
}

void PhenylRuntime::pluginFrameBegin () {
    PHENYL_TRACE(LOGGER, "Running plugin frameBegin()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running frameBegin() for {}", plugin->getName());
        plugin->frameBegin(*this);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin frameBegin()");
}

void PhenylRuntime::pluginUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin update()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running update() for {}", plugin->getName());
        plugin->update(*this, deltaTime);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin update()");
}

void PhenylRuntime::pluginRender (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin render()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running render() for {}", plugin->getName());
        plugin->render(*this);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin render()");
}

void PhenylRuntime::pluginFixedUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin fixedUpdate()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running fixedUpdate() for {}", plugin->getName());
        plugin->fixedUpdate(*this, deltaTime);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin fixedUpdate()");
}

void PhenylRuntime::pluginPhysicsUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin physicsUpdate()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running physicsUpdate() for {}", plugin->getName());
        plugin->physicsUpdate(*this, deltaTime);
    }
    PHENYL_TRACE(LOGGER, "Finished running plugin physicsUpdate()");
}

void PhenylRuntime::shutdown () {
    PHENYL_LOGI(LOGGER, "Shutting down runtime!");

    PHENYL_TRACE(LOGGER, "Clearing entities");
    manager().clear();

    PHENYL_TRACE(LOGGER, "Running plugin shutdown()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running shutdown() for {}", plugin->getName());
        plugin->shutdown(*this);
    }

    PHENYL_TRACE(LOGGER, "Clearing rest of component manager");
    manager().clearAll(); // TODO: try to get rid of this

    PHENYL_TRACE(LOGGER, "Destructing plugins");
    plugins.clear();
}
