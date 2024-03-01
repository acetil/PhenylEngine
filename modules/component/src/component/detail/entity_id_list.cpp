#include <cassert>
#include "component/detail/entity_id_list.h"
#include "component/entity_id.h"
#include "logging/logging.h"

using namespace phenyl::component::detail;

static phenyl::Logger LOGGER{"ENTITY_ID_LIST"};

EntityIdList::EntityIdList (std::size_t capacity) : idSlots{}, freeListStart{FREE_LIST_EMPTY}, numEntities{0} {
    idSlots.reserve(capacity);
}

phenyl::component::EntityId EntityIdList::newId () {
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
    if (id.id == 0 || id.id > idSlots.size()) {
        return false;
    }

    return (idSlots[id.id - 1] & EMPTY_BIT) == 0 && idSlots[id.id - 1] == id.generation;
}

void EntityIdList::removeId (EntityId id) {
    assert(id && id.id <= idSlots.size());
    assert(check(id));

    idSlots[id.id - 1] |= EMPTY_BIT | (freeListStart << GEN_BITS);
    freeListStart = id.id;
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

EntityIdList::IdIterator::IdIterator (const detail::EntityIdList* idList, std::size_t slotPos) : idList{idList}, slotPos{slotPos} {
    if (slotPos != idList->idSlots.size() && (idList->idSlots[slotPos] & EMPTY_BIT)) {
        next();
    }
}

void EntityIdList::IdIterator::next () {
    if (slotPos == idList->idSlots.size()) {
        return;
    }

    while (++slotPos < idList->idSlots.size() && (idList->idSlots[slotPos] & EMPTY_BIT)) {}
}

void EntityIdList::IdIterator::prev () {
    if (slotPos == 0) {
        return;
    }

    while (--slotPos > 0 && (idList->idSlots[slotPos] & EMPTY_BIT)) {}
}

EntityIdList::IdIterator::value_type EntityIdList::IdIterator::operator* () const {
    PHENYL_DASSERT((idList->idSlots[slotPos] & EMPTY_BIT) == 0);
    return EntityId{static_cast<unsigned int>(idList->idSlots[slotPos]), static_cast<unsigned int>(slotPos + 1)};
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
    return idList == other.idList && slotPos == other.slotPos;
}

