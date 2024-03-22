#pragma once

#include <concepts>

#include "component/component.h"
#include "component/component_serializer.h"

#include "iresource.h"

namespace phenyl::runtime {
    class IPlugin;

    class PhenylRuntime {
    private:
        component::ComponentManager compManager;
        component::EntitySerializer serializationManager;

        std::vector<std::unique_ptr<IResource>> ownedResources;

        util::Map<std::size_t, IResource*> resources;
        util::Map<std::size_t, std::unique_ptr<IPlugin>> plugins;


        void registerResource (std::size_t typeIndex, IResource* resource);
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
            auto* resPtr = resourceMaybe<T>();
            PHENYL_ASSERT_MSG(resPtr, "Attempted to get resource that does not exist!");

            return *resPtr;
        }

        template <std::derived_from<IResource> T>
        const T& resource () const {
            const auto* resPtr = resourceMaybe<T>();
            PHENYL_ASSERT_MSG(resPtr, "Attempted to get resource that does not exist!");

            return *resPtr;
        }

        template <std::derived_from<IResource> T>
        T* resourceMaybe () {
            auto typeIndex = meta::type_index<T>();
            return resources.contains(typeIndex) ? static_cast<T*>(resources[typeIndex]) : nullptr;
        }

        template <std::derived_from<IResource> T>
        const T* resourceMaybe () const {
            auto typeIndex = meta::type_index<T>();
            return resources.contains(typeIndex) ? static_cast<const T*>(resources[typeIndex]) : nullptr;
        }

        template <std::derived_from<IResource> T, typename ...Args>
        void addResource (Args&&... args) requires (std::constructible_from<T, Args...>) {
            auto& res = ownedResources.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
            addResource<T>((T*)res.get());
        }

        template <std::derived_from<IResource> T>
        void addResource (T* resource) {
            registerResource(meta::type_index<T>(), resource);
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

        template <common::CustomSerializable T>
        void addComponent () {
            addUnserializedComponent<T>();
            serializer().addSerializer<T>();
        }

        template <typename T>
        void addUnserializedComponent () {
            manager().addComponent<T>();
        }

        void pluginPostInit ();

        void pluginFrameBegin ();
        void pluginUpdate (double deltaTime);
        void pluginRender (double deltaTime);

        void pluginFixedUpdate (double deltaTime);
        void pluginPhysicsUpdate (double deltaTime);
    };
}