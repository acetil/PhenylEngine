#include <cassert>
#include "core/component/detail/entity_id_list.h"
#include "core/entity_id.h"
#include "core/detail/loggers.h"
#include "logging/logging.h"

using namespace phenyl::core::detail;

static phenyl::Logger LOGGER{"ENTITY_ID_LIST", phenyl::core::detail::COMPONENT_LOGGER};

EntityIdList::EntityIdList (std::size_t capacity) : idSlots{}, freeListStart{FREE_LIST_EMPTY}, numEntities{0} {
    idSlots.reserve(capacity);
}

phenyl::core::EntityId EntityIdList::newId () {
    if (freeListStart == FREE_LIST_EMPTY) {
        if (idSlots.size() >= MAX_NUM_IDS) {
            PHENYL_LOGE(LOGGER, "Too many entity ids!");
            return EntityId{};
        }
        idSlots.push_back(1);
        numEntities++;

        PHENYL_DASSERT(idSlots.size() < (std::size_t{1} << FREE_LIST_BITS));

        return EntityId{1, static_cast<unsigned int>(idSlots.size())};
    } else {
        auto index = freeListStart - 1;

        PHENYL_DASSERT(idSlots.size() > index);
        PHENYL_DASSERT(EMPTY_BIT & idSlots.at(index));

        freeListStart = (idSlots.at(index) & FREE_LIST_MASK) >> GEN_BITS;
        auto gen = static_cast<unsigned int>(idSlots.at(index) & GEN_MASK);
        gen++;
        if (gen == 0) {
            gen = 1;
        }

        idSlots.at(index) = gen;
        numEntities++;

        return EntityId{gen, static_cast<unsigned int>(index + 1)};
    }
}

bool EntityIdList::check (EntityId id) const {
    if (id.m_id == 0 || id.m_id > idSlots.size()) {
        return false;
    }

    return (idSlots[id.m_id - 1] & EMPTY_BIT) == 0 && idSlots[id.m_id - 1] == id.m_generation;
}

void EntityIdList::removeId (EntityId id) {
    PHENYL_DASSERT(id && id.m_id <= idSlots.size());
    PHENYL_DASSERT(check(id));

    idSlots[id.m_id - 1] |= EMPTY_BIT | (freeListStart << GEN_BITS);
    freeListStart = id.m_id;
    numEntities--;
}

void EntityIdList::clear () {
    if (!idSlots.size()) {
        return;
    }

    for (std::size_t i = 0; i < idSlots.size() - 1; i++) {
        auto gen = idSlots[i] & GEN_MASK;
        gen++;
        if (gen == 0) {
            gen = 1;
        }

        idSlots[i] = EMPTY_BIT | ((i + 2) << GEN_BITS) | gen;
    }

    auto gen = idSlots[idSlots.size() - 1] & GEN_MASK;
    gen++;
    if (gen == 0) {
        gen = 1;
    }

    idSlots[idSlots.size() - 1] = EMPTY_BIT | (FREE_LIST_EMPTY << GEN_BITS) | gen;
    freeListStart = 1;
    numEntities = 0;
}

std::size_t EntityIdList::size () const {
    return numEntities;
}

std::size_t EntityIdList::maxIndex () const {
    return idSlots.size();
}

EntityIdList::iterator EntityIdList::begin () const {
    return cbegin();
}

EntityIdList::const_iterator EntityIdList::cbegin () const {
    return const_iterator{this, 0};
}

EntityIdList::iterator EntityIdList::end () const {
    return cend();
}

EntityIdList::const_iterator EntityIdList::cend () const {
    return const_iterator{this, idSlots.size()};
}

EntityIdList::IdIterator::IdIterator (const detail::EntityIdList* idList, std::size_t slotPos) : m_idList{idList}, m_pos{slotPos} {
    if (slotPos != idList->idSlots.size() && (idList->idSlots[slotPos] & EMPTY_BIT)) {
        next();
    }
}

void EntityIdList::IdIterator::next () {
    if (m_pos == m_idList->idSlots.size()) {
        return;
    }

    while (++m_pos < m_idList->idSlots.size() && (m_idList->idSlots[m_pos] & EMPTY_BIT)) {}
}

void EntityIdList::IdIterator::prev () {
    if (m_pos == 0) {
        return;
    }

    while (--m_pos > 0 && (m_idList->idSlots[m_pos] & EMPTY_BIT)) {}
}

EntityIdList::IdIterator::value_type EntityIdList::IdIterator::operator* () const {
    PHENYL_DASSERT((m_idList->idSlots[m_pos] & EMPTY_BIT) == 0);
    return EntityId{static_cast<unsigned int>(m_idList->idSlots[m_pos]), static_cast<unsigned int>(m_pos + 1)};
}

EntityIdList::IdIterator& EntityIdList::IdIterator::operator++ () {
    next();
    return *this;
}

EntityIdList::IdIterator EntityIdList::IdIterator::operator++ (int) {
    auto copy = *this;
    ++*this;

    return copy;
}

EntityIdList::IdIterator& EntityIdList::IdIterator::operator-- () {
    prev();
    return *this;
}

EntityIdList::IdIterator EntityIdList::IdIterator::operator-- (int) {
    auto copy = *this;
    --*this;

    return copy;
}

bool EntityIdList::IdIterator::operator== (const detail::EntityIdList::IdIterator& other) const {
    return m_idList == other.m_idList && m_pos == other.m_pos;
}

