#include "logging/logging.h"
#include "util/random.h"

#include "runtime/plugin.h"
#include "runtime/resource_manager.h"
#include "runtime/runtime.h"

#include "runtime/delta_time.h"

using namespace phenyl::runtime;

static phenyl::Logger LOGGER{"RUNTIME", phenyl::PHENYL_LOGGER};

void ResourceManager::registerResource (std::size_t typeIndex, IResource* resource) {
    PHENYL_ASSERT_MSG(!resources.contains(typeIndex), "Attempted to add resource \"{}\" but has already been added!", resource->getName());

    resources.emplace(typeIndex, resource);
    PHENYL_LOGI(LOGGER, "Registered resource \"{}\"", resource->getName());
}

PhenylRuntime::PhenylRuntime (phenyl::component::ComponentManager&& compManager) : compManager{std::move(compManager)} {
    PHENYL_LOGI(LOGGER, "Initialised Phenyl runtime");
    addResource<DeltaTime>();
    addResource<FixedDelta>();
}

PhenylRuntime::~PhenylRuntime () = default;

void PhenylRuntime::registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin) {
    PHENYL_DASSERT(!plugins.contains(typeIndex));
    PHENYL_TRACE(LOGGER, "Starting registration of plugin \"{}\"", plugin->getName());

    auto& pluginRef = *plugin;

    plugins.emplace(typeIndex, std::move(plugin));

    pluginRef.init(*this);
    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", pluginRef.getName());
}

void PhenylRuntime::executeSystems (std::vector<std::unique_ptr<IRunnableSystem>>& systems) {
    // TODO: cache topo ordering
    std::unordered_set<IRunnableSystem*> executedSystems;
    std::unordered_set<IRunnableSystem*> executingSystems;
    PHENYL_DEBUG({
        util::Random::Shuffle(systems.begin(), systems.end());
    })
    for (const auto& i : systems) {
        i->execute(manager(), resourceManager, executedSystems, executingSystems);
    }
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

    manager().defer();
    executeSystems(postInitSystems);
    manager().deferEnd();

    PHENYL_TRACE(LOGGER, "Finished running plugin postInit()");
}

void PhenylRuntime::pluginFrameBegin () {
    PHENYL_TRACE(LOGGER, "Running plugin frameBegin()");
    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running frameBegin() for {}", plugin->getName());
        plugin->frameBegin(*this);
    }

    manager().defer();
    executeSystems(frameBeginSystems);
    manager().deferEnd();

    PHENYL_TRACE(LOGGER, "Finished running plugin frameBegin()");
}

void PhenylRuntime::pluginUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin update()");
    resource<DeltaTime>().set(deltaTime);

    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running update() for {}", plugin->getName());
        plugin->update(*this, deltaTime);
    }

    manager().defer();
    executeSystems(updateSystems);
    manager().deferEnd();

    PHENYL_TRACE(LOGGER, "Finished running plugin update()");
}

void PhenylRuntime::pluginRender (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin render()");
    resource<DeltaTime>().set(deltaTime);

    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running render() for {}", plugin->getName());
        plugin->render(*this);
    }

    manager().defer();
    executeSystems(renderSystems);
    manager().deferEnd();

    PHENYL_TRACE(LOGGER, "Finished running plugin render()");
}

void PhenylRuntime::pluginFixedUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin fixedUpdate()");
    resource<FixedDelta>().set(deltaTime);

    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running fixedUpdate() for {}", plugin->getName());
        plugin->fixedUpdate(*this, deltaTime);
    }

    manager().defer();
    executeSystems(fixedUpdateSystems);
    manager().deferEnd();

    PHENYL_TRACE(LOGGER, "Finished running plugin fixedUpdate()");
}

void PhenylRuntime::pluginPhysicsUpdate (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Running plugin physicsUpdate()");
    resource<FixedDelta>().set(deltaTime);

    for (auto [_, plugin] : plugins.kv()) {
        PHENYL_TRACE(LOGGER, "Running physicsUpdate() for {}", plugin->getName());
        plugin->physicsUpdate(*this, deltaTime);
    }

    manager().defer();
    executeSystems(physicsSystems);
    manager().deferEnd();

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

void IRunnableSystem::execute (component::ComponentManager& manager, ResourceManager& resManager, std::unordered_set<IRunnableSystem*>& executedSystems, std::unordered_set<IRunnableSystem*>& executingSystems) {
    PHENYL_ASSERT_MSG(!executingSystems.contains(this), "Detected cycle in system run order!");

    if (executedSystems.contains(this)) {
        return;
    }

    executingSystems.emplace(this);
    for (auto* i : parentSystems) {
        i->execute(manager, resManager, executedSystems, executingSystems);
    }

    run(manager, resManager);
    executedSystems.emplace(this);
    executingSystems.erase(this);
}
