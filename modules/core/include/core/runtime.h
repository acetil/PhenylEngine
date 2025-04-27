#pragma once

#include <concepts>

#include "core/world.h"
#include "core/serialization/component_serializer.h"
#include "util/set.h"

#include "plugin.h"
#include "core/runtime/resource_manager.h"
#include "runtime/stage.h"
#include "stages.h"
#include "runtime/system.h"

namespace phenyl::core {
    class IPlugin;

    class PhenylRuntime {
    private:
        World m_world;
        core::EntityComponentSerializer m_serializer;

        ResourceManager m_resourceManager;

        util::Set<std::size_t> m_initPlugins;
        util::Map<std::size_t, std::unique_ptr<IPlugin>> m_plugins;

        std::unordered_map<std::string, std::unique_ptr<IRunnableSystem>> m_systems;
        std::unordered_map<std::size_t, std::unique_ptr<AbstractStage>> m_stages;

        void registerPlugin (std::size_t typeIndex, IInitPlugin& plugin);
        void registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin);

        template <typename S, typename ...Args>
        System<S>* makeSystem (std::string systemName, void (*systemFunc)(Args...)) {
            PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            std::unique_ptr<System<S>> system = MakeSystem<S>(std::move(systemName), systemFunc, world(), m_resourceManager);
            auto* ptr = system.get();
            m_systems[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T, typename ...Args>
        System<S>* makeSystem (std::string systemName, void (T::*systemFunc)(Args...)) {
            PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            std::unique_ptr<System<S>> system = MakeSystem<S>(std::move(systemName), systemFunc, world(), m_resourceManager);
            auto* ptr = system.get();
            m_systems[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T>
        System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)(PhenylRuntime&)) {
            PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName), [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(runtime); });
            auto* ptr = system.get();
            m_systems[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T>
        System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)()) {
            PHENYL_DASSERT_MSG(!m_systems.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName), [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(); });
            auto* ptr = system.get();
            m_systems[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S>
        Stage<S>* getStage () {
            auto typeIndex = meta::type_index<S>();

            auto it = m_stages.find(typeIndex);
            if (it != m_stages.end()) {
                return static_cast<Stage<S>*>(it->second.get());
            } else {
                return nullptr;
            }
        }

        template <typename S>
        const Stage<S>* getStage () const {
            auto typeIndex = meta::type_index<S>();

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
    public:
        explicit PhenylRuntime ();
        virtual ~PhenylRuntime();

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

        template <std::derived_from<IResource> T, typename ...Args>
        void addResource (Args&&... args) requires (std::constructible_from<T, Args...>) {
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
        void addPlugin () requires std::is_default_constructible_v<T> {
            auto typeIndex = meta::type_index<T>();
            if (m_plugins.contains(typeIndex)) {
                // Do not add plugins twice
                return;
            }

            registerPlugin(typeIndex, std::make_unique<T>());
        }

        template <std::derived_from<IInitPlugin> T>
        void addPlugin () requires std::is_default_constructible_v<T> {
            auto typeIndex = meta::type_index<T>();
            if (m_initPlugins.contains(typeIndex)) {
                // Do not add plugins twice
                return;
            }

            T plugin{};
            registerPlugin(typeIndex, plugin);
        }

        template <std::derived_from<IPlugin> T>
        void registerPlugin (std::unique_ptr<T> plugin) {
            registerPlugin(meta::type_index<T>(), std::move(plugin));
        }

        template <core::SerializableType T>
        void addComponent (std::string name) {
            addUnserializedComponent<T>(std::move(name));
            serializer().addSerializer<T>();
        }

        template <typename T>
        void addUnserializedComponent (std::string name) {
            world().addComponent<T>(std::move(name));
        }

        template <typename S>
        System<S>& addSystem (std::string systemName, auto systemFunc) {
            auto* system = makeSystem<S>(std::move(systemName), systemFunc);
            auto* stage = getStage<S>();
            PHENYL_ASSERT(stage);
            stage->addSystem(system);

            return *system;
        }

        template <typename S, typename T, typename ...Args>
        System<S>& addSystem (std::string systemName, T* obj, void (T::*systemFunc)(Args...)) {
            auto* stage = getStage<S>();
            PHENYL_ASSERT(stage);

            auto* system = makeSystem<S, T>(std::move(systemName), obj, systemFunc);
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
        void runFixedTimestep (double deltaTime);
        void runVariableTimestep (double deltaTime);
        void runRender ();

        void shutdown ();

        template <typename S>
        friend class Stage;
        friend class IRunnableSystem;
    };
}