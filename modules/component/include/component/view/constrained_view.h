#pragma once

#include <tuple>
#include <utility>

#include "component/component_new.h"

namespace component::view {

    namespace detail {
        template<std::size_t MaxComponents, typename ...Args>
        class ConstrainedViewIterator;
    }

    template <std::size_t MaxComponents, typename ...Args>
    class ConstrainedView;

    template <std::size_t MaxComponents, typename ...Args>
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
        ConstrainedEntityView<MaxComponents, Args2...> constrain () {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>, "All requested types must be accessible!");
            /*if constexpr (meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>) {
                return util::Optional<ConstrainedEntityView<Args2...>>(ConstrainedEntityView(entityId, std::get<Args2...>(comps)));
            } else {
                return util::NullOpt;
            }*/
            return {entityId, std::get<Args2...>(comps)};
        }

        friend ComponentManagerNew<MaxComponents>;
        friend ConstrainedView<MaxComponents, Args...>;
        friend detail::ConstrainedViewIterator<MaxComponents, Args...>;
    };

    template <std::size_t MaxComponents, typename ...Args>
    class ConstrainedView {
    private:
        std::tuple<util::Bitfield<MaxComponents>*, component::EntityId*, Args*...> comps{};
        typename component::ComponentManagerNew<MaxComponents>::SharedPtr componentManager{nullptr};
        util::Bitfield<MaxComponents> mask{};
        ConstrainedView () = default;
        std::size_t numObjects () {
            return componentManager->getNumObjects();
        }
    public:
        using iterator = detail::ConstrainedViewIterator<MaxComponents, Args...>;
        ConstrainedView (typename component::ComponentManagerNew<MaxComponents>::SharedPtr _compManager, util::Bitfield<MaxComponents>* bitfields, component::EntityId* ids, Args*... compPtrs,
                         util::Bitfield<MaxComponents> mask = {}) : componentManager{std::move(_compManager)}, comps{bitfields, ids, compPtrs...}, mask{mask} {}

        util::Optional<ConstrainedEntityView<MaxComponents, Args...>> getEntityView (component::EntityId entityId) const {
            if (!componentManager) {
                return util::NullOpt;
            }

            return componentManager->tempGetPos(entityId).then([this](const size_t& pos) -> util::Optional<ConstrainedEntityView<MaxComponents, Args...>> {
                if ((std::get<util::Bitfield<MaxComponents>*>(comps)[pos] & mask) == mask) {
                    return {ConstrainedEntityView<MaxComponents, Args...>{std::get<EntityId*>(comps)[pos],
                                                                         std::get<Args*>(comps)[pos]...}};
                } else {
                    return util::NullOpt;
                }
            });
        }

        template <typename ...Args2>
        ConstrainedView<MaxComponents, Args2...> constrain () const {
            static_assert(meta::is_all_in<meta::type_list_wrapper<Args...>, Args2...>, "All requested types must be accessible!");

            return ConstrainedView<MaxComponents, Args2...>(componentManager, std::get<util::Bitfield<MaxComponents>*>(comps), std::get<EntityId*>(comps), std::get<Args2*>(comps)..., mask);
        }

        inline iterator begin ();
        inline iterator end ();

        friend detail::ConstrainedViewIterator<MaxComponents, Args...>;
        friend ComponentManagerNew<MaxComponents>;
    };

    namespace detail {
        template<std::size_t MaxComponents, typename ...Args>
        class ConstrainedViewIterator {
        private:
            ConstrainedView<MaxComponents, Args...> constrainedView;
            std::size_t pos{};

            bool isValidObject (std::size_t checkPos) {
                auto val = std::get<util::Bitfield<MaxComponents>*>(constrainedView.comps)[checkPos];
                return (val & constrainedView.mask) == constrainedView.mask;
            }

            void findFirst () {
                while (pos < constrainedView.numObjects() && !isValidObject(pos)) {
                    pos++;
                }
            }

            void findNext () {
                pos++;

                while (pos < constrainedView.numObjects() && !isValidObject(pos)) {
                    pos++;
                }
            }

            void findPrev () {
                pos--;
                while (pos >= 0 && !isValidObject(pos)) {
                    pos--;
                }
            }

        public:
            ConstrainedViewIterator () = default;

            ConstrainedViewIterator (ConstrainedView<MaxComponents, Args...> _constrainedView, std::size_t startPos) : constrainedView {
                    _constrainedView}, pos{startPos} {
                findFirst();
            }

            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = ConstrainedEntityView<MaxComponents, Args...>;
            using difference_type = std::ptrdiff_t;

            value_type operator* () const {
                return ConstrainedEntityView<MaxComponents, Args...>(std::get<component::EntityId*>(constrainedView.comps)[pos],
                                                      std::get<Args*>(constrainedView.comps)[pos]...);
            }

            ConstrainedViewIterator<MaxComponents, Args...>& operator++ () {
                findNext();
                return *this;
            }

            ConstrainedViewIterator<MaxComponents, Args...> operator++ (int) {
                ConstrainedViewIterator other = *this;
                findNext();
                return other;
            }

            ConstrainedViewIterator<MaxComponents, Args...>& operator-- () {
                findPrev();
                return *this;
            }

            ConstrainedViewIterator<MaxComponents, Args...> operator-- (int) {
                ConstrainedViewIterator other = *this;
                findPrev();
                return other;
            }

            bool operator== (const ConstrainedViewIterator<MaxComponents, Args...>& other) const {
                return pos == other.pos || (!constrainedView.componentManager && !other.constrainedView.componentManager);
            }
        };

    }
    template <std::size_t N, typename ...Args>
    inline typename ConstrainedView<N, Args...>::iterator ConstrainedView<N, Args...>::begin () {
        return {*this, 0};
    }

    template <std::size_t N, typename ...Args>
    inline typename ConstrainedView<N, Args...>::iterator ConstrainedView<N, Args...>::end () {
        if (!componentManager) {
            return begin();
        }
        return {*this, componentManager->getNumObjects()};
    }
}