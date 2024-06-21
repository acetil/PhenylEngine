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

        std::vector<std::unique_ptr<AbstractSystem>> postInitSystems;
        std::vector<std::unique_ptr<AbstractSystem>> frameBeginSystems;
        std::vector<std::unique_ptr<AbstractSystem>> updateSystems;
        std::vector<std::unique_ptr<AbstractSystem>> renderSystems;

        std::vector<std::unique_ptr<AbstractSystem>> fixedUpdateSystems;
        std::vector<std::unique_ptr<AbstractSystem>> physicsSystems;

        void registerPlugin (std::size_t typeIndex, IInitPlugin& plugin);
        void registerPlugin (std::size_t typeIndex, std::unique_ptr<IPlugin> plugin);
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

        template <typename Stage, typename ...Args>
        void addSystem (void (*systemFunc)(Args...)) {
            std::unique_ptr<AbstractSystem> system = MakeSystem(systemFunc, manager(), resourceManager);
            //system->init(manager(), resourceManager);

            if constexpr (std::is_same_v<Stage, PostInit>) {
                postInitSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, FrameBegin>) {
                frameBeginSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, Update>) {
                updateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, Render>) {
                renderSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, FixedUpdate>) {
                fixedUpdateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, PhysicsUpdate>) {
                physicsSystems.emplace_back(std::move(system));
            } else {
                static_assert(false);
            }
        }

        template <typename Stage, typename T, typename ...Args>
        void addSystem (void (T::*systemFunc)(Args...)) {
            std::unique_ptr<AbstractSystem> system = MakeSystem(systemFunc, manager(), resourceManager);
            //system->init(manager(), resourceManager);

            if constexpr (std::is_same_v<Stage, PostInit>) {
                postInitSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, FrameBegin>) {
                frameBeginSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, Update>) {
                updateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, Render>) {
                renderSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, FixedUpdate>) {
                fixedUpdateSystems.emplace_back(std::move(system));
            } else if constexpr (std::is_same_v<Stage, PhysicsUpdate>) {
                physicsSystems.emplace_back(std::move(system));
            } else {
                static_assert(false);
            }
        }

        void pluginPostInit ();

        void pluginFrameBegin ();
        void pluginUpdate (double deltaTime);
        void pluginRender (double deltaTime);

        void pluginFixedUpdate (double deltaTime);
        void pluginPhysicsUpdate (double deltaTime);

        void shutdown ();
    };
}