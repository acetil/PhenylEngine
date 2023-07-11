#include "component/component_new.h"

using namespace component;

detail::ComponentSet::ComponentSet (std::size_t startCapacity, std::size_t compSize) : ids{}, indexSet{}, data{std::make_unique<std::byte[]>(startCapacity * compSize)},
        compSize{compSize}, dataSize{0}, dataCapacity{startCapacity} {
    ids.reserve(startCapacity);
    indexSet.reserve(startCapacity);
}

detail::ComponentSet::~ComponentSet () = default;

void detail::ComponentSet::guaranteeEntityIndex (std::size_t index) {
    while (indexSet.size() < index) {
        indexSet.push_back(EMPTY_INDEX);
    }
}

std::byte* detail::ComponentSet::getComponentUntyped (EntityId id) const {
    assert(indexSet.size() > id.id - 1);

    auto compIndex = indexSet[id.id - 1];
    if (compIndex == EMPTY_INDEX) {
        return nullptr;
    } else {
        return data.get() + (compSize * compIndex);
    }
}

std::byte* detail::ComponentSet::tryInsert (EntityId id) {
    assert(indexSet.size() > id.id - 1);

    if (indexSet[id.id - 1] != EMPTY_INDEX) {
        return nullptr;
    }

    guaranteeCapacity(dataSize + 1);

    auto* ptr = data.get() + (compSize * dataSize);
    indexSet[id.id - 1] = dataSize;
    ids.push_back(id);

    dataSize++;
    return ptr;
}

void detail::ComponentSet::deleteComp (EntityId id) {
    assert(indexSet.size() > id.id - 1);
    if (indexSet[id.id - 1] == EMPTY_INDEX) {
        return;
    }

    auto compIndex = indexSet[id.id - 1];
    auto* compPtr = data.get() + compSize * compIndex;
    if (compIndex == dataSize - 1) {
        deleteTypedComp(compPtr);
    } else {
        auto oldCompPtr = data.get() + compSize * (dataSize - 1);
        moveTypedComp(compPtr, oldCompPtr);

        auto movedId = ids[dataSize - 1];
        ids[compIndex] = movedId;
        indexSet[movedId.id - 1] = compIndex;
    }

    indexSet[id.id - 1] = EMPTY_INDEX;
    ids.pop_back();
    dataSize--;
}

void detail::ComponentSet::guaranteeCapacity (std::size_t capacity) {
    if (dataCapacity >= capacity) {
        return;
    }

    while (dataCapacity < capacity) {
        dataCapacity *= RESIZE_FACTOR;
    }

    auto newData = std::make_unique<std::byte[]>(dataCapacity * compSize);
    moveAllComps(newData.get(), data.get(), dataSize);

    data = std::move(newData);
}

void detail::ComponentSet::clear () {
    for (std::size_t i = 0; i < dataSize; i++) {
        deleteTypedComp(data.get() + (i * compSize));
        auto index = ids[i].id - 1;

        indexSet[index] = EMPTY_INDEX;
        ids[i] = EntityId{};
    }
}

bool detail::ComponentSet::hasComp (EntityId id) const {
    assert(indexSet.size() > id.id - 1);
    return indexSet[id.id - 1] != EMPTY_INDEX;
}


