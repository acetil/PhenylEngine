#pragma once

#include <concepts>
#include <functional>
#include <unordered_set>

#include "component/component.h"

#include "forward.h"
#include "resources.h"
#include "resource_manager.h"

namespace phenyl::runtime {
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
    };

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, Components&...), component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), &resManager, func] () mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, Components&... components) {
                func(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () mutable {
            query.each([&] (auto entity, Components&... components) {
                func(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (component::Entity, const Resources<ResourceTypes...>&, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (component::Entity entity, Components&... components) {
                func(entity, resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (component::Entity, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () mutable {
            query.each([&] (component::Entity entity, Components&... components) mutable {
                func(entity, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, const component::QueryBundle<Components...>&, const component::QueryBundle<Components...>&),
        component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.pairs([&] (const component::QueryBundle<Components...>& bundle1, const component::QueryBundle<Components...>& bundle2) mutable {
                func(resources, bundle1, bundle2);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const component::QueryBundle<Components...>&, const component::QueryBundle<Components...>&),
        component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentDoubleSystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () mutable {
            query.pairs([&] (const component::QueryBundle<Components...>& bundle1, const component::QueryBundle<Components...>& bundle2) mutable {
                func(bundle1, bundle2);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        component::Query<T, std::remove_reference_t<Components>...> query = manager.query<T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func, &resManager] () mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, T& obj, Components&... components) {
                (obj.*func)(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () mutable {
            query.each([&] (auto entity, T& obj, Components&... components) {
                (obj.*func)(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, Components...) const, component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<const T, std::remove_reference_t<Components>...>();
        std::function<void()>func1 = [query = std::move(query), func, &resManager] () mutable{
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, const T& obj, Components&... components) {
                (obj.*func)(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (Components...) const, component::ComponentManager& manager, ResourceManager& resManager) {
        auto query = manager.query<const T, std::remove_reference_t<Components>...>();
        std::function<void()> func1 = [query = std::move(query), func] () mutable {
            query.each([&] (auto entity, const T& obj, Components&... components) {
                (obj.*func)(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ResourceSystem<Resources...>>(func);

        std::function<void()> func1 = [func, &resManager] () {
            func(Resources<ResourceTypes...>{resManager});
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }
}
