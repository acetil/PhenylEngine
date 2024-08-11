#pragma once

#include <cassert>
#include <iterator>
#include <vector>

#include "component/entity_id.h"

#include "logging/logging.h"
#include "util/iterable.h"

namespace phenyl::component::detail {
    class RelationshipManager {
    private:
        struct Relationship {
            EntityId parent{};
            EntityId children{};
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
            const RelationshipManager* manager;
            EntityId curr;
            ChildIterator (const RelationshipManager* manager, EntityId curr) : manager{manager}, curr{curr} {}

            friend class RelationshipManager;
        public:
            using value_type = EntityId;
            using reference = const value_type&;
            using difference_type = std::ptrdiff_t;

            ChildIterator () : manager{nullptr}, curr{} {};

            reference operator* () const {
                return curr;
            }

            ChildIterator& operator++ () {
                curr = manager->getRelationship(curr).next;
                return *this;
            }
            ChildIterator operator++ (int) {
                auto it = *this;
                ++*this;
                return it;
            }

            bool operator== (const ChildIterator& other) const {
                return manager == other.manager && curr == other.curr;
            }
        };
        std::vector<Relationship> relationships{};
        Relationship& getRelationship (EntityId id) {
            PHENYL_DASSERT(id.id < relationships.size());

            return relationships[id.id];
        }

        [[nodiscard]] const Relationship& getRelationship (EntityId id) const {
            PHENYL_DASSERT(id.id < relationships.size());

            return relationships[id.id];
        }

        friend class ChildIterator;
    public:
        using ChildIterator = ChildIterator;
        static_assert(std::forward_iterator<ChildIterator>);

        explicit RelationshipManager (std::size_t startCapacity) {
            relationships.reserve(startCapacity + 1);
            relationships.push_back(Relationship{});
        }

        void add (EntityId id, EntityId parent) {
            PHENYL_DASSERT(relationships.size() >= id.id);
            if (relationships.size() <= id.id) {
                relationships.push_back(Relationship{});
            }

            setParent(id, parent);
        }

        void setParent (EntityId id, EntityId parent) {
            getRelationship(id).parent = parent;
            auto oldStart = getRelationship(parent).children;

            getRelationship(id).next = oldStart;

            if (oldStart) {
                getRelationship(oldStart).prev = id;
            }
            getRelationship(parent).children = id;
        }

        void removeFromParent (EntityId id) {
            auto& rel = getRelationship(id);

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
            relationships.clear();
            relationships.push_back(Relationship{});
        }
    };
}