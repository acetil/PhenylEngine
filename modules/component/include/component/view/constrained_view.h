#pragma once

#include <tuple>
#include <utility>

#include "component/component_new.h"

namespace component::view {

    namespace detail {
        template<typename ...Args>
        class ConstrainedViewIterator;
    }

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
        friend detail::ConstrainedViewIterator<Args...>;
    };

    template <typename ...Args>
    class ConstrainedView {
    private:
        std::tuple<component::EntityId*, Args*...> comps{};
        component::ComponentManagerNew::SharedPtr componentManager{nullptr};
        ConstrainedView () = default;
    public:
        using iterator = detail::ConstrainedViewIterator<Args...>;
        ConstrainedView (component::ComponentManagerNew::SharedPtr _compManager, component::EntityId* ids, Args*... compPtrs) : componentManager{std::move(_compManager)}, comps{ids, compPtrs...} {}

        util::Optional<ConstrainedEntityView<Args...>> getEntityView (component::EntityId entityId) const {
            if (!componentManager) {
                return util::NullOpt;
            }

            return componentManager->tempGetPos(entityId).thenMap([this](const size_t& pos){
                return ConstrainedEntityView<Args...>{std::get<EntityId*>(comps)[pos], std::get<Args*>(comps)[pos]...};
            });
        }

        template <typename ...Args2>
        ConstrainedView<Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>, "All requested types must be accessible!");

            return ConstrainedView<Args2...>(componentManager, std::get<EntityId*>(comps), std::get<Args2*>(comps)...);
        }

        inline iterator begin ();
        inline iterator end ();

        friend detail::ConstrainedViewIterator<Args...>;
        friend ComponentManagerNew;
    };

    namespace detail {
        template<typename ...Args>
        class ConstrainedViewIterator {
        private:
            ConstrainedView<Args...> constrainedView;
            std::size_t pos{};

            void findFirst () {

            }

            void findNext () {
                pos++;
            }

            void findPrev () {
                pos--;
            }

        public:
            ConstrainedViewIterator () = default;

            ConstrainedViewIterator (ConstrainedView<Args...> _constrainedView, std::size_t startPos) : constrainedView {
                    _constrainedView}, pos{startPos} {
                findFirst();
            }

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = ConstrainedEntityView<Args...>;
            using difference_type = std::ptrdiff_t;

            value_type operator* () const {
                return ConstrainedEntityView<Args...>(std::get<component::EntityId*>(constrainedView.comps)[pos],
                                                      std::get<Args*>(constrainedView.comps)[pos]...);
            }

            ConstrainedViewIterator<Args...>& operator++ () {
                findNext();
                return *this;
            }

            ConstrainedViewIterator<Args...> operator++ (int) {
                ConstrainedViewIterator other = *this;
                findNext();
                return other;
            }

            ConstrainedViewIterator<Args...>& operator-- () {
                findPrev();
                return *this;
            }

            ConstrainedViewIterator<Args...> operator-- (int) {
                ConstrainedViewIterator other = *this;
                findPrev();
                return other;
            }

            bool operator== (const ConstrainedViewIterator<Args...>& other) const {
                return pos == other.pos || (!constrainedView.componentManager && !other.constrainedView.componentManager);
            }
        };

    }
    template <typename ...Args>
    inline typename ConstrainedView<Args...>::iterator ConstrainedView<Args...>::begin () {
        return {*this, 0};
    }

    template <typename ...Args>
    inline typename ConstrainedView<Args...>::iterator ConstrainedView<Args...>::end () {
        if (!componentManager) {
            return begin();
        }
        return {*this, componentManager->getNumObjects()};
    }
}