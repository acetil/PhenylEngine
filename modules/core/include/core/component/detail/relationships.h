#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "core/entity_id.h"

#include "logging/logging.h"
#include "util/iterable.h"

namespace phenyl::core::detail {
    class RelationshipManager {
    private:
        struct Relationship {
            EntityId parent{};
            EntityId children{};

            // Intrusive doubly linked list
            EntityId next{};
            EntityId prev{};

            void clear () {
                parent = {};
                children = {};
                next = {};
                prev = {};
            }
        };

        class ChildIterator {
        private:
            const RelationshipManager* m_manager;
            EntityId m_curr;
            ChildIterator (const RelationshipManager* manager, EntityId curr) : m_manager{manager}, m_curr{curr} {}

            friend class RelationshipManager;
        public:
            using value_type = EntityId;
            using reference = const value_type&;
            using difference_type = std::ptrdiff_t;

            ChildIterator () : m_manager{nullptr}, m_curr{} {};

            reference operator* () const {
                return m_curr;
            }

            ChildIterator& operator++ () {
                m_curr = m_manager->getRelationship(m_curr).next;
                return *this;
            }
            ChildIterator operator++ (int) {
                auto it = *this;
                ++*this;
                return it;
            }

            bool operator== (const ChildIterator& other) const {
                return m_manager == other.m_manager && m_curr == other.m_curr;
            }
        };
        std::vector<Relationship> m_relationships{};
        Relationship& getRelationship (EntityId id) {
            PHENYL_DASSERT(id.m_id < m_relationships.size());

            return m_relationships[id.m_id];
        }

        [[nodiscard]] const Relationship& getRelationship (EntityId id) const {
            PHENYL_DASSERT(id.m_id < m_relationships.size());

            return m_relationships[id.m_id];
        }

        friend class ChildIterator;
    public:
        using ChildIterator = ChildIterator;
        static_assert(std::forward_iterator<ChildIterator>);

        explicit RelationshipManager (std::size_t startCapacity) {
            m_relationships.reserve(startCapacity + 1);
            m_relationships.push_back(Relationship{});
        }

        void add (EntityId id, EntityId parent) {
            PHENYL_DASSERT(m_relationships.size() >= id.m_id);
            if (m_relationships.size() <= id.m_id) {
                m_relationships.push_back(Relationship{});
            }

            setParent(id, parent);
        }

        void setParent (EntityId id, EntityId parent) {
            getRelationship(id).parent = parent;

            // Add to start of linked list
            auto oldStart = getRelationship(parent).children;

            getRelationship(id).next = oldStart;
            if (oldStart) {
                getRelationship(oldStart).prev = id;
            }
            getRelationship(parent).children = id;
        }

        void removeFromParent (EntityId id) {
            auto& rel = getRelationship(id);

            // Remove from doubly linked list
            if (rel.prev) {
                getRelationship(rel.prev).next = rel.next;
            } else {
                getRelationship(rel.parent).children = rel.next;
            }

            if (rel.next) {
                getRelationship(rel.next).prev = rel.prev;
            }

            rel.prev = EntityId{};
            rel.next = EntityId{};
        }

        void remove (EntityId id, bool updateParent) {
            if (updateParent) {
                removeFromParent(id);
            }

            getRelationship(id).clear();
        }

        [[nodiscard]] EntityId parent (EntityId id) const {
            return getRelationship(id).parent;
        }

        EntityId entityChildren (EntityId id) const {
            return getRelationship(id).children;
        }

        EntityId next (EntityId id) const {
            return getRelationship(id).next;
        }

        [[nodiscard]] util::Iterable<ChildIterator> children (EntityId parent) const {
            return {childrenBegin(parent), childrenEnd(parent)};
        }

        [[nodiscard]] ChildIterator childrenBegin (EntityId parent) const {
            return ChildIterator{this, getRelationship(parent).children};
        }

        [[nodiscard]] ChildIterator childrenEnd (EntityId parent) const {
            return ChildIterator{this, EntityId{}};
        }

        void reset () {
            m_relationships.clear();
            m_relationships.push_back(Relationship{});
        }
    };
}