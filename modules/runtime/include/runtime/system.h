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
        void execute (component::ComponentManager& manager, ResourceManager& resManager, std::unordered_set<IRunnableSystem*>& executedSystems, std::unordered_set<IRunnableSystem*>& executingSystems);
    protected:
        std::unordered_set<IRunnableSystem*> parentSystems;
        std::string systemName;

        // TODO
        virtual void run (component::ComponentManager& manager, ResourceManager& resManager) {}
    public:
        explicit IRunnableSystem (std::string name) : systemName{std::move(name)} {}

        virtual ~IRunnableSystem () = default;

        const std::string& getName () const noexcept {
            return systemName;
        }

        virtual void run (PhenylRuntime& runtime);

        //friend class PhenylRuntime;
        friend class AbstractStage;
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
        std::function<void(component::ComponentManager& manager, ResourceManager& resManager)> func;
    public:
        explicit FunctionSystem (std::string name, std::function<void(component::ComponentManager& manager, ResourceManager& resManager)> func) : System<Stage>{std::move(name)}, func{std::move(func)} {}

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            func(manager, resManager);
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

    template <typename Stage, typename P>
    class PluginFunctionSystem : public System<Stage> {
    private:
        P* plugin;
        std::function<void(P&, PhenylRuntime&)> systemFunc;
    public:
        PluginFunctionSystem (std::string name, P* plugin, std::function<void(P&, PhenylRuntime&)> systemFunc) : System<Stage>{std::move(name)}, plugin{plugin}, systemFunc{std::move(systemFunc)} {}

        void run (PhenylRuntime& runtime) override {
            systemFunc(plugin, runtime);
        }
    };

    /*template <ResourceType ...Resources, ComponentType ...Components> requires (sizeof...(Components) > 0)
    class ComponentSystem : public AbstractSystem {
    private:
        void (*func) (Resources&... resources, Components...);
        component::Query<std::remove_reference_t<Components>...> query{};
    public:
        explicit ComponentSystem (void (*func) (Resources&... resources, Components...)) : func{func} {}

        void init (component::ComponentManager& manager, ResourceManager& resManager) override {
            query = manager.query<std::remove_reference_t<Components>...>();
        }

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            auto res = std::make_tuple(resManager.resource<Resources>()...);

            query.each([&] (auto entity, Components... components) {
                func(std::get<Resources&...>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
            });
        }
    };

    template <ResourceType ...Resources, ComponentType ...Components> requires (sizeof...(Components) > 0)
    class ComponentEntitySystem : public AbstractSystem {
    private:
        void (*func) (component::Entity, Resources&..., Components...);
        component::Query<std::remove_reference_t<Components>...> query{};
    public:
        explicit ComponentEntitySystem (void (*func) (component::Entity, Resources&..., Components...)) : func{func} {}

        void init (component::ComponentManager& manager, ResourceManager& resManager) override {
            query = manager.query<std::remove_reference_t<Components>...>();
        }

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            auto res = std::make_tuple(resManager.resource<Resources>()...);

            query.each([&] (component::Entity entity, Components... components) {
                func(entity, std::get<Resources&...>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
            });
        }
    };

    template <ResourceType ...Resources, ComponentType ...Components> requires (sizeof...(Components) > 0)
    class ComponentDoubleSystem : public AbstractSystem {
    private:
        void (*func) (Resources&... resources, const component::ConstQueryBundle<Components...>&, const component::ConstQueryBundle<Components...>&);
        component::Query<std::remove_reference_t<Components>...> query{};
    public:
        explicit ComponentDoubleSystem (void (*func) (Resources&... resources, const component::ConstQueryBundle<Components...>&, const component::ConstQueryBundle<Components...>&)) : func{func} {}

        void init (component::ComponentManager& manager, ResourceManager& resManager) override {
            query = manager.query<Components...>();
        }

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            auto res = std::make_tuple(resManager.resource<Resources>()...);

            query.pairs([&] (const component::ConstQueryBundle<Components...>& bundle1, const component::ConstQueryBundle<Components...>& bundle2) {
                func(std::get<Resources&...>(res)..., bundle1, bundle2);
            });
        }
    };

    template <ComponentType T, ResourceType ...Resources, ComponentType ...Components>
    class MethodSystem : public AbstractSystem {
    private:
        void (T::*func) (Resources&..., Components...);
        component::Query<T, std::remove_reference_t<Components>...> query{};
    public:
        explicit MethodSystem (void (T::*func) (Resources&... resources, Components...)) : func{func} {}

        void init (component::ComponentManager& manager, ResourceManager& resManager) override {
            query = manager.query<T, std::remove_reference_t<Components>...>();
        }

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            auto res = std::make_tuple(resManager.resource<Resources>()...);

            query.each([&] (T& obj, Components... components) {
                (obj.func)(std::get<Resources&...>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
            });
        }
    };

    template <ResourceType ...Resources>
    class ResourceSystem : public AbstractSystem {
    private:
        void (*func) (Resources&...);
    public:
        explicit ResourceSystem (void (*func) (Resources&...)) : func{func} {}

        void init (component::ComponentManager& manager, ResourceManager& resManager) override {}

        void run (component::ComponentManager& manager, ResourceManager& resManager) override {
            func(resManager.resource<Resources>()...);
        }
    };*/


    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, Components&...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentSystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, Components&... components) {
                func(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentSystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            query.each([&] (auto entity, Components&... components) {
                func(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (component::Entity, const Resources<ResourceTypes...>&, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentEntitySystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (component::Entity entity, Components&... components) {
                /*if constexpr (sizeof...(Resources) > 0) {
                    func(entity, std::get<Resources&>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
                } else {
                    func(entity, std::forward<std::remove_reference_t<Components>>(components)...);
                }*/
                func(entity, resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (component::Entity, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentEntitySystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            query.each([&] (component::Entity entity, Components&... components) mutable {
                /*if constexpr (sizeof...(Resources) > 0) {
                    func(entity, std::get<Resources&>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
                } else {
                    func(entity, std::forward<std::remove_reference_t<Components>>(components)...);
                }*/
                func(entity, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes, ComponentType ...Components> requires (sizeof...(Components) > 0 && (!std::same_as<std::remove_all_extents_t<Components>, component::Entity> && ... && true))
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&, const component::QueryBundle<Components...>&, const component::QueryBundle<Components...>&),
        component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ComponentDoubleSystem<Resources..., Components...>>(func);

        auto query = manager.query<std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.pairs([&] (const component::QueryBundle<Components...>& bundle1, const component::QueryBundle<Components...>& bundle2) mutable {
                /*if constexpr (sizeof...(Resources) > 0) {
                    func(std::get<Resources&>(res)..., bundle1, bundle2);
                } else {
                    func(bundle1, bundle2);
                }*/
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
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            query.pairs([&] (const component::QueryBundle<Components...>& bundle1, const component::QueryBundle<Components...>& bundle2) mutable {
                /*if constexpr (sizeof...(Resources) > 0) {
                    func(std::get<Resources&>(res)..., bundle1, bundle2);
                } else {
                    func(bundle1, bundle2);
                }*/
                func(bundle1, bundle2);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<MethodSystem<T, Resources..., Components...>>(func);

        component::Query<T, std::remove_reference_t<Components>...> query = manager.query<T, std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, T& obj, Components&... components) {
                /*if constexpr (sizeof...(Resources) > 0) {
                    (obj.func)(std::get<Resources&>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
                } else {
                    (obj.func)(std::forward<std::remove_reference_t<Components>>(components)...);
                }*/
                (obj.*func)(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (Components...), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<MethodSystem<T, Resources..., Components...>>(func);

        auto query = manager.query<T, std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            query.each([&] (auto entity, T& obj, Components&... components) {
                (obj.*func)(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ResourceType ...ResourceTypes, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (const Resources<ResourceTypes...>& resources, Components...) const, component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<MethodSystem<T, Resources..., Components...>>(func);

        auto query = manager.query<const T, std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable{
            Resources<ResourceTypes...> resources{resManager};

            query.each([&] (auto entity, const T& obj, Components&... components) {
                /*if constexpr (sizeof...(Resources) > 0) {
                    (obj.func)(std::get<Resources&>(res)..., std::forward<std::remove_reference_t<Components>>(components)...);
                } else {
                    (obj.func)(std::forward<std::remove_reference_t<Components>>(components)...);
                }*/
                (obj.*func)(resources, components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ComponentType T, ComponentType ...Components>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (T::*func) (Components...) const, component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<MethodSystem<T, Resources..., Components...>>(func);

        auto query = manager.query<const T, std::remove_reference_t<Components>...>();
        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [query = std::move(query), func] (component::ComponentManager& manager, ResourceManager& resManager) mutable {
            query.each([&] (auto entity, const T& obj, Components&... components) {
                (obj.*func)(components...);
            });
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }

    template <typename Stage, ResourceType ...ResourceTypes>
    std::unique_ptr<System<Stage>> MakeSystem (std::string systemName, void (*func) (const Resources<ResourceTypes...>&), component::ComponentManager& manager, ResourceManager& resManager) {
        //return std::make_unique<ResourceSystem<Resources...>>(func);

        std::function<void(component::ComponentManager&, ResourceManager&)> func1 = [func] (component::ComponentManager& manager, ResourceManager& resManager) {
            func(Resources<ResourceTypes...>{resManager});
        };

        return std::make_unique<FunctionSystem<Stage>>(std::move(systemName), std::move(func1));
    }
}
