#pragma once

#include <concepts>
#include <functional>
#include <unordered_set>

#include "core/world.h"

#include "forward.h"
#include "core/resources.h"
#include "resource_manager.h"

namespace phenyl::core {
    class IRunnableSystem {
    private:

    protected:
        std::unordered_set<IRunnableSystem*> parentSystems;
        std::string systemName;
    public:
        explicit IRunnableSystem (std::string name) : systemName{std::move(name)} {}

        virtual ~IRunnableSystem () = default;

        const std::string& getName () const noexcept {
            return systemName;
        }

        virtual void run (PhenylRuntime& runtime) = 0;
        virtual bool exclusive () const noexcept {
            return false;
        }

        const std::unordered_set<IRunnableSystem*>& getPrecedingSystems () const {
            return parentSystems;
        }
    };

    template <typename Stage>
    class System : public IRunnableSystem {
    public:
        explicit System (std::string name) : IRunnableSystem{std::move(name)} {}

        System<Stage>& runBefore (System<Stage>& otherSystem) {
            otherSystem.runAfter(*this);

            return *this;
        }

        System<Stage>& runAfter (System<Stage>& otherSystem) {
            PHENYL_DASSERT(!otherSystem.parentSystems.contains(this));
            parentSystems.emplace(&otherSystem);

            return *this;
        }
    };

    template <typename Stage>
    class FunctionSystem : public System<Stage> {
    private:
        std::function<void()> func;
    public:
        explicit FunctionSystem (std::string name, std::function<void()> func) : System<Stage>{std::move(name)}, func{std::move(func)} {}

        void run (PhenylRuntime& runtime) override {
            func();
        }

        bool exclusive () const noexcept override {
            return true;
        }
    };

    template <typename Stage, typename T>
    class ExclusiveFunctionSystem : public System<Stage> {
    private:
        std::function<void(PhenylRuntime&)> func;
    public:
        ExclusiveFunctionSystem (std::string name, std::function<void(PhenylRuntime&)> func) : System<Stage>{std::move(name)}, func{std::move(func)} {}

        void run (PhenylRuntime& runtime) override {
            func(runtime);
        }

        bool exclusive () const noexcept override {
            return true;
        }
    };

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, core::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, Components&...), World& world, ResourceManager& resManager) {
        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), &resManager, func] () {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (Components&... components) {
                func(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, core::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (Components...), World& world, ResourceManager& resManager) {
        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () {
            query.each([&] (Components&... components) {
                func(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, core::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, const core::Bundle<Components...>& bundle), World& world, ResourceManager& resManager) {
        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (const core::Bundle<Components...>& bundle) {
                func(resources, bundle);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    /*template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity2> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (component::Entity2, Components...), component::World& world, ResourceManager& resManager) {
        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () {
            query.each([&] (component::Entity2 entity, Components&... components) {
                func(entity, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }*/

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, core::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, const core::Bundle<Components...>&, const core::Bundle<Components...>&),
        World& world, ResourceManager& resManager) {
        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () {
            Resources<ResourceTypes...> resources{resManager};

            query.pairs([&] (const core::Bundle<Components...>& bundle1, const core::Bundle<Components...>& bundle2) {
                func(resources, bundle1, bundle2);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, core::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const core::Bundle<Components...>&, const core::Bundle<Components...>&),
        World& world, ResourceManager& resManager) {
        //return std::make_unique<ComponentDoubleSystem<Resources..., Components...>>(func);

        auto query = world.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () {
            query.pairs([&] (const core::Bundle<Components...>& bundle1, const core::Bundle<Components...>& bundle2) {
                func(bundle1, bundle2);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, Components...), World& world, ResourceManager& resManager) {
        core::Query<T, std::remove_reference_t<Components>...> query = world.query<T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (T& obj, Components&... components) {
                (obj.*func)(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (Components...), World& world, ResourceManager& resManager) {
        auto query = world.query<T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () {
            query.each([&] (T& obj, Components&... components) {
                (obj.*func)(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, const phenyl::core::Bundle<Components...>& bundle), World& world, ResourceManager& resManager) {
        auto query = world.query<T, std::remove_reference_t<Components>...>();
        std::function<void()>func1 = [query = std::move(query), func, &resManager] () {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (const phenyl::core::Bundle<T, Components...>& bundle) {
                T& obj = bundle.template get<T>();
                (obj.*func)(resources, bundle.template subset<Components...>());
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const phenyl::core::Bundle<Components...>& bundle), World& world, ResourceManager& resManager) {
        auto query = world.query<T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () {
            query.each([&] (const phenyl::core::Bundle<T, Components...>& bundle) {
                T& obj = bundle.template get<T>();
                (obj.*func)(bundle.template subset<Components...>());
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&), World& world, ResourceManager& resManager) {
        //return std::make_unique<ResourceSystem<Resources...>>(func);

        std::function<void()> func1 = [func, &resManager] () {
            func(Resources<ResourceTypes...>{resManager});
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }
}
