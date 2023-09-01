#pragma once

#include <cstddef>
#include <vector>

#include "component/entity_id.h"

namespace component::detail {
    class EntityIdList {
    private:
        static constexpr std::size_t GEN_BITS = sizeof(unsigned int) * 8 - 1;
        static constexpr std::size_t NUM_GENS = std::size_t{1} << GEN_BITS;
        static constexpr std::size_t GEN_MASK = (std::size_t{1} << GEN_BITS) - 1;
        static constexpr std::size_t FREE_LIST_BITS = sizeof(std::size_t) * 8 - GEN_BITS - 1;
        static constexpr std::size_t FREE_LIST_MASK = ((std::size_t{1} << FREE_LIST_BITS) - 1) << GEN_BITS;
        static constexpr std::size_t FREE_LIST_EMPTY = 0;
        static constexpr std::size_t MAX_NUM_IDS = (std::size_t{1} << (sizeof(unsigned int) * 8)) - 1;
        static constexpr std::size_t EMPTY_BIT = std::size_t{1} << (sizeof(std::size_t) * 8 - 1);

        class IdIterator {
        private:
            const EntityIdList* idList;
            std::size_t slotPos;
            explicit IdIterator (const EntityIdList* idList, std::size_t slotPos);

            void next ();
            void prev ();
        public:
            using value_type = EntityId;
            using reference = void;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            IdIterator () : idList{nullptr}, slotPos{0} {}

            value_type operator* () const;

            IdIterator& operator++ ();
            IdIterator operator++ (int);

            IdIterator& operator-- ();
            IdIterator operator-- (int);

            bool operator== (const IdIterator& other) const;

            friend class EntityIdList;
        };

        std::vector<std::size_t> idSlots;
        std::size_t numEntities;
        std::size_t freeListStart;
    public:
        using const_iterator = IdIterator;
        using iterator = const_iterator;
        static_assert(std::bidirectional_iterator<iterator>);
        static_assert(std::bidirectional_iterator<const_iterator>);

        explicit EntityIdList (std::size_t capacity);

        EntityId newId ();
        [[nodiscard]] bool check (EntityId id) const;

        void removeId (EntityId id);
        void clear ();

        [[nodiscard]] std::size_t size () const;
        [[nodiscard]] std::size_t maxIndex () const;

        [[nodiscard]] iterator begin () const;
        [[nodiscard]] const_iterator cbegin () const;

        [[nodiscard]] iterator end () const;
        [[nodiscard]] const_iterator cend () const;
    };
}