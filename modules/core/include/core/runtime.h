#pragma once

#include "core/runtime/resource_manager.h"
#include "core/serialization/component_serializer.h"
#include "core/world.h"
#include "plugin.h"
#include "runtime/introspection.h"
#include "runtime/stage.h"
#include "runtime/system.h"
#include "stages.h"

#include <concepts>

namespace phenyl::core {
class IPlugin;

class PhenylRuntime {
public:
    explicit PhenylRuntime ();
    virtual ~PhenylRuntime ();

    World& world () {
        return m_world;
    }

    [[nodiscard]] const World& world () const {
        return m_world;
    }

    core::EntityComponentSerializer& serializer () {
        return m_serializer;
    }

    template <std::derived_from<IResource> T>
    T& resource () {
        return m_resourceManager.resource<T>();
    }

    template <std::derived_from<IResource> T>
    const T& resource () const {
        return m_resourceManager.resource<T>();
    }

    template <std::derived_from<IResource> T>
    T* resourceMaybe () {
        return m_resourceManager.resourceMaybe<T>();
    }

    template <std::derived_from<IResource> T>
    const T* resourceMaybe () const {
        return m_resourceManager.resourceMaybe<T>();
    }

    template <std::derived_from<IResource> T, typename... Args>
    void addResource (Args&&... args) requires (std::constructible_from<T, Args...>)
    {
        m_resourceManager.addResource<T>(std::forward<Args>(args)...);
    }

    template <std::derived_from<IResource> T>
    void addResource (T&& res) {
        m_resourceManager.addResource<T>(std::forward<T>(res));
    }

    template <std::derived_from<IResource> T>
    void addResource (T* resource) {
        m_resourceManager.addResource(resource);
    }

    ResourceManager& resources () {
        return m_resourceManager;
    }

    template <std::derived_from<IPlugin> T>
    void addPlugin () requires std::is_default_constructible_v<T>
    {
        auto typeIndex = meta::TypeIndex::Get<T>();
        if (m_plugins.contains(typeIndex)) {
            // Do not add plugins twice
            return;
        }

        registerPlugin(typeIndex, std::make_unique<T>());
    }

    template <std::derived_from<IInitPlugin> T>
    void addPlugin () requires std::is_default_constructible_v<T>
    {
        auto typeIndex = meta::TypeIndex::Get<T>();
        if (m_initPlugins.contains(typeIndex)) {
            // Do not add plugins twice
            return;
        }

        T plugin{};
        registerPlugin(typeIndex, plugin);
    }

    template <std::derived_from<IPlugin> T>
    void registerPlugin (std::unique_ptr<T> plugin) {
        registerPlugin(meta::TypeIndex::Get<T>(), std::move(plugin));
    }

    template <core::SerializableType T>
    void addComponent (std::string name) {
        addUnserializedComponent<T>(name);
        serializer().addSerializer<T>();
        m_componentInfos.emplace_back(ComponentInfo::GetInfo<T>(std::move(name)));
    }

    template <typename T>
    void addUnserializedComponent (std::string name) {
        world().addComponent<T>(std::move(name));
    }

    template <typename Interface, std::derived_from<Interface> T>
    void declareInterface () {
        world().declareInterface<Interface, T>();
    }

    template <typename S>
    System<S>& addSystem (std::string systemName, auto systemFunc) {
        auto* system = makeSystem<S>(std::move(systemName), systemFunc);
        auto* stage = getStage<S>();
        PHENYL_ASSERT(stage);
        stage->addSystem(system);

        return *system;
    }

    template <typename S, typename T, typename... Args>
    System<S>& addSystem (std::string systemName, T* obj, void (T::*systemFunc)(Args...)) {
        auto* stage = getStage<S>();
        PHENYL_ASSERT(stage);

        auto* system = makeSystem<S, T>(std::move(systemName), obj, systemFunc);
        stage->addSystem(system);

        return *system;
    }

    template <typename S, typename... Args>
    System<S>& addHierarchicalSystem (std::string systemName, void (*systemFunc)(Args...)) {
        auto* stage = getStage<S>();
        PHENYL_ASSERT(stage);

        auto* system = makeHierarchicalSystem<S>(std::move(systemName), systemFunc);
        stage->addSystem(system);

        return *system;
    }

    template <typename S, typename Parent>
    void addStage (std::string name) {
        auto* parent = getStage<Parent>();
        PHENYL_ASSERT(parent);

        auto stage = initStage<S>(std::move(name));
        parent->addChildStage(stage);
    }

    template <typename Before, typename After>
    void runStageBefore () {
        auto* before = getStage<Before>();
        auto* after = getStage<After>();
        PHENYL_DASSERT(before);
        PHENYL_DASSERT(after);

        before->runBefore(after);
    }

    void runPostInit ();
    void runFrameBegin ();
    void runFixedTimestep ();
    void runVariableTimestep ();
    void runRender ();

    void shutdown ();

    const std::vector<ComponentInfo>& components () const noexcept;
    const std::vector<std::string>& plugins () const noexcept;

private:
    World m_world;
    core::EntityComponentSerializer m_serializer;
    std::vector<ComponentInfo> m_componentInfos;

    ResourceManager m_resourceManager;

    std::unordered_set<meta::TypeIndex> m_initPlugins;
    std::unordered_map<meta::TypeIndex, std::unique_ptr<IPlugin>> m_plugins;
    std::vector<std::string> m_pluginNames;

    std::unordered_map<std::string, std::unique_ptr<IRunnableSystem>> m_systems;
    std::unordered_map<meta::TypeIndex, std::unique_ptr<AbstractStage>> m_stages;

    void registerPlugin (meta::TypeIndex typeIndex, IInitPlugin& plugin);
    void registerPlugin (meta::TypeIndex typeIndex, std::unique_ptr<IPlugin> plugin);

    template <typename S, typename... Args>
    System<S>* makeSystem (std::string systemName, void (*systemFunc)(Args...)) {
        PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"",
            systemName);
        std::unique_ptr<System<S>> system =
            MakeSystem<S>(std::move(systemName), systemFunc, world(), m_resourceManager);
        auto* ptr = system.get();
        m_systems[ptr->getName()] = std::move(system);

        return ptr;
    }

    template <typename S, typename T, typename... Args>
    System<S>* makeSystem (std::string systemName, void (T::*systemFunc)(Args...)) {
        PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"",
            systemName);
        std::unique_ptr<System<S>> system =
            MakeSystem<S>(std::move(systemName), systemFunc, world(), m_resourceManager);
        auto* ptr = system.get();
        m_systems[ptr->getName()] = std::move(system);

        return ptr;
    }

    template <typename S, typename T>
    System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)(PhenylRuntime&)) {
        PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"",
            systemName);
        auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName),
            [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(runtime); });
        auto* ptr = system.get();
        m_systems[ptr->getName()] = std::move(system);

        return ptr;
    }

    template <typename S, typename T>
    System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)()) {
        PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"",
            systemName);
        auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName),
            [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(); });
        auto* ptr = system.get();
        m_systems[ptr->getName()] = std::move(system);

        return ptr;
    }

    template <typename S, typename... Args>
    System<S>* makeHierarchicalSystem (std::string systemName, void (*systemFunc)(Args...)) {
        PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"",
            systemName);
        auto system = MakeHierachicalSystem<S>(std::move(systemName), systemFunc, world(), m_resourceManager);
        auto* ptr = system.get();
        m_systems[ptr->getName()] = std::move(system);

        return ptr;
    }

    template <typename S>
    Stage<S>* getStage () {
        auto typeIndex = meta::TypeIndex::Get<S>();

        auto it = m_stages.find(typeIndex);
        if (it != m_stages.end()) {
            return static_cast<Stage<S>*>(it->second.get());
        } else {
            return nullptr;
        }
    }

    template <typename S>
    const Stage<S>* getStage () const {
        auto typeIndex = meta::TypeIndex::Get<S>();

        auto it = m_stages.find(typeIndex);
        if (it != m_stages.end()) {
            return static_cast<Stage<S>*>(it->second.get());
        } else {
            return nullptr;
        }
    }

    template <typename S>
    Stage<S>* initStage (std::string name) {
        auto stage = std::make_unique<Stage<S>>(std::move(name), *this);
        auto* ptr = stage.get();
        m_stages.emplace(ptr->id(), std::move(stage));

        return ptr;
    }

    template <typename S>
    friend class Stage;
    friend class IRunnableSystem;
};
} // namespace phenyl::core
