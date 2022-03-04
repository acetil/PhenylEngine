#pragma once

#include <tuple>
#include <utility>

#include "component/component_new.h"

namespace component::view {

    template <typename ...Args>
    class ConstrainedView;

    template <typename ...Args>
    class ConstrainedEntityView {
    private:
        std::tuple<Args&...> comps;
        component::EntityId entityId;
        explicit ConstrainedEntityView (component::EntityId _entityId, Args&... args) : entityId{_entityId}, comps{args...} {}
    public:
        ConstrainedEntityView (ConstrainedEntityView&) = default;
        ConstrainedEntityView (ConstrainedEntityView&&)  noexcept = default;

        template <typename T>
        T& get () const {
            return std::get<T&>(comps);
        }

        [[nodiscard]] component::EntityId getId () const {
            return entityId;
        }

        template <typename ...Args2>
        util::Optional<ConstrainedEntityView<Args2...>> constrain () {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>, "All requested types must be accessible!");
            /*if constexpr (meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>) {
                return util::Optional<ConstrainedEntityView<Args2...>>(ConstrainedEntityView(entityId, std::get<Args2...>(comps)));
            } else {
                return util::NullOpt;
            }*/
            return ConstrainedEntityView(entityId, std::get<Args2...>(comps));
        }

        friend ComponentManagerNew;
        friend ConstrainedView<Args...>;
    };

    template <typename ...Args>
    class ConstrainedView {
    private:
        std::tuple<component::EntityId*, Args*...> comps;
        component::ComponentManagerNew::SharedPtr componentManager;
    public:
        ConstrainedView (component::ComponentManagerNew::SharedPtr _compManager, component::EntityId* ids, Args*... compPtrs) : componentManager{std::move(_compManager)}, comps{ids, compPtrs...} {}

        util::Optional<ConstrainedEntityView<Args...>> getEntityView (component::EntityId entityId) const {
            return componentManager->tempGetPos(entityId).thenMap([this](const size_t& pos){
                return ConstrainedEntityView<Args...>{std::get<EntityId*>(comps)[pos], std::get<Args*>(comps)[pos]...};
            });
        }

        template <typename ...Args2>
        ConstrainedView<Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>, "All requested types must be accessible!");

            return ConstrainedView<Args2...>(componentManager, std::get<EntityId*>(comps), std::get<Args2*>(comps)...);
        }
    };
}