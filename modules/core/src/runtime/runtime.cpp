#include "core/runtime.h"

#include "core/delta_time.h"
#include "core/iresource.h"
#include "core/runtime/resource_manager.h"
#include "core/stages.h"
#include "logging/logging.h"
#include "util/random.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"RUNTIME", phenyl::PHENYL_LOGGER};

void ResourceManager::registerResource (std::size_t typeIndex, IResource* resource) {
    PHENYL_ASSERT_MSG(!m_resources.contains(typeIndex), "Attempted to add resource \"{}\" but has already been added!",
        resource->getName());

    m_resources.emplace(typeIndex, resource);
    PHENYL_LOGI(LOGGER, "Registered resource \"{}\"", resource->getName());
}

PhenylRuntime::PhenylRuntime () : m_world{} {
    PHENYL_LOGI(LOGGER, "Initialised Phenyl runtime");
    initStage<PostInit>("PostInit");
    initStage<FrameBegin>("FrameBegin");
    initStage<GlobalFixedTimestep>("GlobalFixedTimestep");
    initStage<GlobalVariableTimestep>("GlobalVariableTimestep");
    initStage<Render>("Render");

    addStage<FixedUpdate, GlobalFixedTimestep>("FixedUpdate");
    addStage<PhysicsUpdate, GlobalFixedTimestep>("PhysicsUpdate");
    runStageBefore<FixedUpdate, PhysicsUpdate>();

    addStage<Update, GlobalVariableTimestep>("Update");

    addResource<DeltaTime>();
    addResource<FixedDelta>();
}

PhenylRuntime::~PhenylRuntime () = default;

void PhenylRuntime::registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin) {
    PHENYL_DASSERT(!m_plugins.contains(typeIndex));
    PHENYL_TRACE(LOGGER, "Starting registration of plugin \"{}\"", plugin->getName());

    auto& pluginRef = *plugin;

    m_plugins.emplace(typeIndex, std::move(plugin));

    pluginRef.init(*this);
    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", pluginRef.getName());
}

void PhenylRuntime::registerPlugin (std::size_t typeIndex, IInitPlugin& plugin) {
    PHENYL_DASSERT(!m_initPlugins.contains(typeIndex));
    PHENYL_TRACE(LOGGER, "Starting registration of init plugin \"{}\"", plugin.getName());

    m_initPlugins.emplace(typeIndex);

    plugin.init(*this);
    PHENYL_LOGI(LOGGER, "Registered plugin \"{}\"", plugin.getName());
}

void PhenylRuntime::runPostInit () {
    PHENYL_TRACE(LOGGER, "Initiating PostInit stage");
    getStage<PostInit>()->run();
}

void PhenylRuntime::runFrameBegin () {
    PHENYL_TRACE(LOGGER, "Initiating FrameBegin stage");
    getStage<FrameBegin>()->run();
}

void PhenylRuntime::runFixedTimestep (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Initiating GlobalFixedTimestep stage");
    resource<FixedDelta>().set(deltaTime);
    getStage<GlobalFixedTimestep>()->run();
}

void PhenylRuntime::runVariableTimestep (double deltaTime) {
    PHENYL_TRACE(LOGGER, "Initating GlobalVariableTimestep stage");
    resource<DeltaTime>().set(deltaTime);
    getStage<GlobalVariableTimestep>()->run();
}

void PhenylRuntime::runRender () {
    PHENYL_TRACE(LOGGER, "Initating Render stage");
    getStage<Render>()->run();
}

void PhenylRuntime::shutdown () {
    PHENYL_LOGI(LOGGER, "Shutting down runtime!");

    PHENYL_TRACE(LOGGER, "Clearing entities");
    world().clear();

    PHENYL_TRACE(LOGGER, "Running plugin shutdown()");
    for (const auto& plugin : m_plugins | std::views::values) {
        PHENYL_TRACE(LOGGER, "Running shutdown() for {}", plugin->getName());
        plugin->shutdown(*this);
    }

    // PHENYL_TRACE(LOGGER, "Clearing rest of component manager");
    // manager().clearAll(); // TODO: try to get rid of this

    PHENYL_TRACE(LOGGER, "Destructing plugins");
    m_plugins.clear();
}
