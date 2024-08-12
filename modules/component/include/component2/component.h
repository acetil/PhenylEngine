#pragma once

#include "component/detail/entity_id_list.h"
#include "component/entity_id.h"
#include "component/detail/relationships.h"
#include "component/detail/loggers.h"

#include "archetype.h"
#include "children_view.h"
#include "entity2.h"
#include "query.h"
#include "detail/component_instance.h"
#include "detail/signal_handler.h"

namespace phenyl::component {
    class ComponentManager2 : private detail::IArchetypeManager {
    private:
        std::unordered_map<std::size_t, std::unique_ptr<detail::UntypedComponent>> components;

        detail::EntityIdList idList;
        detail::RelationshipManager relationships;

        std::vector<std::unique_ptr<Archetype>> archetypes;
        EmptyArchetype* emptyArchetype;
        std::vector<detail::EntityEntry> entityEntries;

        std::vector<std::weak_ptr<QueryArchetypes>> queryArchetypes;

        std::unordered_map<std::size_t, std::unique_ptr<detail::IHandlerVector>> signalHandlerVectors;

        void removeInt (EntityId id, bool updateParent);

        std::shared_ptr<QueryArchetypes> makeQueryArchetypes (std::vector<std::size_t> components);
        void cleanupQueryArchetypes ();

        Archetype* findArchetype (const std::vector<std::size_t>& comps) override;
        void updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) override;

        void onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) override;
        void onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) override;

        void raiseSignal (Entity2 entity, std::size_t signalType, const std::byte* ptr);

        friend Entity2;
        friend ChildrenView2;
    public:
        static constexpr std::size_t DEFAULT_CAPACITY = 256;

        explicit ComponentManager2 (std::size_t capacity=DEFAULT_CAPACITY);

        template <typename T>
        void addComponent (std::string name) {
            PHENYL_ASSERT_MSG(!components.contains(meta::type_index<T>()), "Attempted to add component \"{}\" twice", name);

            auto comp = std::make_unique<detail::Component<T>>(this, std::move(name));
            auto index = comp->type();
            components.emplace(index, std::move(comp));
        }

        Entity2 create (EntityId parent);
        void remove (EntityId id);
        void reparent (EntityId id, EntityId parent);

        void clear ();

        [[nodiscard]] bool exists (EntityId id) const noexcept {
            return idList.check(id);
        }

        Entity2 entity (EntityId id) noexcept;
        ChildrenView2 root () noexcept;

        [[nodiscard]] Entity2 parent (EntityId id) noexcept;

        template <typename ...Args>
        Query2<Args...> query () {
            return Query2<Args...>{makeQueryArchetypes(std::vector{meta::type_index<Args>()...}), this};
        }

        template <typename T>
        void addHandler (std::function<void(const OnInsert2<T>&, Entity2)> handler) {
            auto it = components.find(meta::type_index<T>());
            PHENYL_ASSERT_MSG(it != components.end(), "Failed to find component in addHandler()");

            auto& component = static_cast<detail::Component<T>&>(*it->second);
            component.addHandler(std::move(handler));
        }

        template <typename T>
        void addHandler (std::function<void(const OnRemove2<T>&, Entity2)> handler) {
            auto it = components.find(meta::type_index<T>());
            PHENYL_ASSERT_MSG(it != components.end(), "Failed to find component in addHandler()");

            auto& component = static_cast<detail::Component<T>&>(*it->second);
            component.addHandler(std::move(handler));
        }

        template <typename Signal, typename ...Args>
        void addHandler (std::function<void(const Signal&, const Bundle<Args...>& bundle)> handler) {
            detail::SignalHandlerVector<Signal>* handlerVec;
            auto vecIt = signalHandlerVectors.find(meta::type_index<Signal>());
            if (vecIt != signalHandlerVectors.end()) {
                handlerVec = static_cast<detail::SignalHandlerVector<Signal>*>(vecIt->second.get());
            } else {
                auto newVec = std::make_unique<detail::SignalHandlerVector<Signal>>();
                handlerVec = newVec.get();
                signalHandlerVectors.emplace(meta::type_index<Signal>(), std::move(newVec));
            }

            handlerVec->addHandler(std::make_unique<detail::SignalHandler<Signal, Args...>>(query<Args...>(), std::move(handler)));
        }

        template <typename Signal, typename ...Args>
        void addHandler (std::function<void(const Signal&, std::remove_reference_t<Args>&... args)> handler) {
            addHandler(std::function<void(const Signal&, const Bundle<Args...>& bundle)>{[handler = std::move(handler)] (const Signal& signal, const Bundle<Args...>& bundle) {
                handler(signal, std::get<Args>(bundle.comps())...);
            }});
        }
    };
}
