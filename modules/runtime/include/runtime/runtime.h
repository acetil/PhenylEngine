#pragma once

#include <concepts>

#include "component/component.h"
#include "component/component_serializer.h"
#include "util/set.h"

#include "iresource.h"
#include "init_plugin.h"
#include "resource_manager.h"
#include "stages.h"
#include "system.h"

namespace phenyl::runtime {
    class IPlugin;

    class PhenylRuntime {
    private:
        component::ComponentManager compManager;
        component::EntitySerializer serializationManager;

        ResourceManager resourceManager;

        util::Set<std::size_t> initPlugins;
        util::Map<std::size_t, std::unique_ptr<IPlugin>> plugins;

        std::unordered_map<std::string, std::unique_ptr<IRunnableSystem>> systemMap;
        std::unordered_map<std::size_t, std::unique_ptr<AbstractStage>> stageMap;

       /* Stage<PostInit> postInitSystems;
        Stage<FrameBegin> frameBeginSystems;
        Stage<Update> updateSystems;
        Stage<Render> renderSystems;

        Stage<FixedUpdate> fixedUpdateSystems;
        Stage<PhysicsUpdate> physicsSystems;*/

        void registerPlugin (std::size_t typeIndex, IInitPlugin& plugin);
        void registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin);

        //void executeSystems (std::vector<IRunnableSystem*>& systems);

        template <typename S, typename ...Args>
        System<S>* makeSystem (std::string systemName, void (*systemFunc)(Args...)) {
            PHENYL_DASSERT_MSG(!systemMap.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            std::unique_ptr<System<S>> system = MakeSystem<S>(std::move(systemName), systemFunc, manager(), resourceManager);
            auto* ptr = system.get();
            systemMap[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T, typename ...Args>
        System<S>* makeSystem (std::string systemName, void (T::*systemFunc)(Args...)) {
            PHENYL_DASSERT_MSG(!systemMap.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            std::unique_ptr<System<S>> system = MakeSystem<S>(std::move(systemName), systemFunc, manager(), resourceManager);
            auto* ptr = system.get();
            systemMap[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T>
        System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)(PhenylRuntime&)) {
            PHENYL_DASSERT_MSG(!systemMap.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName), [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(runtime); });
            auto* ptr = system.get();
            systemMap[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S, typename T>
        System<S>* makeSystem (std::string systemName, T* obj, void (T::*systemFunc)()) {
            PHENYL_DASSERT_MSG(!systemMap.contains(systemName), "Attempted to add duplicate system with name \"{}\"", systemName);
            auto system = std::make_unique<ExclusiveFunctionSystem<S, T>>(std::move(systemName), [obj, systemFunc] (PhenylRuntime& runtime) { return (obj->*systemFunc)(); });
            auto* ptr = system.get();
            systemMap[ptr->getName()] = std::move(system);

            return ptr;
        }

        template <typename S>
        Stage<S>* getStage () {
            auto typeIndex = meta::type_index<S>();

            auto it = stageMap.find(typeIndex);
            if (it != stageMap.end()) {
                return static_cast<Stage<S>*>(it->second.get());
            } else {
                return nullptr;
            }
        }

        template <typename S>
        const Stage<S>* getStage () const {
            auto typeIndex = meta::type_index<S>();

            auto it = stageMap.find(typeIndex);
            if (it != stageMap.end()) {
                return static_cast<Stage<S>*>(it->second.get());
            } else {
                return nullptr;
            }
        }

        template <typename S>
        Stage<S>* initStage (std::string name) {
            auto stage = std::make_unique<Stage<S>>(std::move(name), *this);
            auto* ptr = stage.get();
            stageMap.emplace(ptr->id(), std::move(stage));

            return ptr;
        }
    public:
        explicit PhenylRuntime (component::ComponentManager&& compManager);
        virtual ~PhenylRuntime();

        component::ComponentManager& manager () {
            return compManager;
        }
        [[nodiscard]] const component::ComponentManager& manager () const {
            return compManager;
        }

        component::EntitySerializer& serializer () {
            return serializationManager;
        }

        template <std::derived_from<IResource> T>
        T& resource () {
            return resourceManager.resource<T>();
        }

        template <std::derived_from<IResource> T>
        const T& resource () const {
            return resourceManager.resource<T>();
        }

        template <std::derived_from<IResource> T>
        T* resourceMaybe () {
            return resourceManager.resourceMaybe<T>();
        }

        template <std::derived_from<IResource> T>
        const T* resourceMaybe () const {
            return resourceManager.resourceMaybe<T>();
        }

        template <std::derived_from<IResource> T, typename ...Args>
        void addResource (Args&&... args) requires (std::constructible_from<T, Args...>) {
            resourceManager.addResource<T>(std::forward<Args>(args)...);
        }

        template <std::derived_from<IResource> T>
        void addResource (T&& res) {
            resourceManager.addResource<T>(std::forward<T>(res));
        }

        template <std::derived_from<IResource> T>
        void addResource (T* resource) {
            resourceManager.addResource(resource);
        }

        ResourceManager& resources () {
            return resourceManager;
        }

        template <std::derived_from<IPlugin> T>
        void addPlugin () requires std::is_default_constructible_v<T> {
            auto typeIndex = meta::type_index<T>();
            if (plugins.contains(typeIndex)) {
                // Do not add plugins twice
                return;
            }

            registerPlugin(typeIndex, std::make_unique<T>());
        }

        template <std::derived_from<IInitPlugin> T>
        void addPlugin () requires std::is_default_constructible_v<T> {
            auto typeIndex = meta::type_index<T>();
            if (initPlugins.contains(typeIndex)) {
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

        template <common::CustomSerializable T>
        void addComponent () {
            addUnserializedComponent<T>();
            serializer().addSerializer<T>();
        }

        template <typename T>
        void addUnserializedComponent () {
            manager().addComponent<T>();
        }

        template <typename S>
        System<S>& addSystem (std::string systemName, auto systemFunc) {
            auto* system = makeSystem<S>(std::move(systemName), systemFunc);

            /*if constexpr (std::is_same_v<S, PostInit>) {
                postInitSystems.addSystem(system);
            } else if constexpr (std::is_same_v<S, FrameBegin>) {
                frameBeginSystems.addSystem(system);
            } else if constexpr (std::is_same_v<S, Update>) {
                updateSystems.addSystem(system);
            } else if constexpr (std::is_same_v<S, Render>) {
                renderSystems.addSystem(system);
            } else if constexpr (std::is_same_v<S, FixedUpdate>) {
                fixedUpdateSystems.addSystem(system);
            } else if constexpr (std::is_same_v<S, PhysicsUpdate>) {
                physicsSystems.addSystem(system);
            } else {
                static_assert(false);
            }*/
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

        /*template <typename S, typename T, typename ...Args>
        System<S>& addSystem (std::string systemName, void (T::*systemFunc)(Args...)) {
            std::unique_ptr<System<S>> system = MakeSystem<S>(std::move(systemName), systemFunc, manager(), resourceManager);
            auto* ptr = system.get();

            if constexpr (std::is_same_v<S, PostInit>) {
                postInitSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<S, FrameBegin>) {
                frameBeginSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<S, Update>) {
                updateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<S, Render>) {
                renderSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<S, FixedUpdate>) {
                fixedUpdateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<S, PhysicsUpdate>) {
                physicsSystems.emplace_back(std::move(system));
            } else {
                static_assert(false);
            }

            return *ptr;
        }*/

        void pluginPostInit ();

        void pluginFrameBegin ();
        void pluginUpdate (double deltaTime);
        void pluginRender (double deltaTime);

        void pluginFixedUpdate (double deltaTime);
        void pluginPhysicsUpdate (double deltaTime);

        void shutdown ();

        template <typename S>
        friend class Stage;
        friend class IRunnableSystem;
    };
}