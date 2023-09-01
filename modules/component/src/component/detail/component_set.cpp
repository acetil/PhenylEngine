#include "component/detail/component_set.h"

using namespace component::detail;

ComponentSet::ComponentSet (std::size_t startCapacity, std::size_t compSize) : ids{}, metadataSet{}, data{compSize != 0 ? std::make_unique<std::byte[]>(startCapacity * compSize) : nullptr},
                                                                               compSize{compSize}, dataSize{0}, dataCapacity{startCapacity}, hierachyDepth{0} {
    ids.reserve(startCapacity);
    metadataSet.reserve(startCapacity);
}

ComponentSet::~ComponentSet () = default;

void ComponentSet::guaranteeEntityIndex (std::size_t index) {
    while (metadataSet.size() < index) {
        metadataSet.push_back({EMPTY_INDEX, nullptr});
    }
}

std::byte* ComponentSet::getComponentUntyped (EntityId id) const {
    assert(metadataSet.size() > id.id - 1);

    /*auto compIndex = metadataSet[id.id - 1];
    if (compIndex == EMPTY_INDEX) {
        return nullptr;
    } else {
        return data.get() + (compSize * compIndex);
    }*/
    return metadataSet[id.id - 1].data;
}

bool ComponentSet::canInsert (component::EntityId id) {
    return !metadataSet[id.id - 1].data && (!parent || parent->canInsert(id));
}

std::byte* ComponentSet::tryInsert (EntityId id) {
    assert(metadataSet.size() > id.id - 1);

    if (compSize == 0 || !canInsert(id)) {
        return nullptr;
    }

    guaranteeCapacity(dataSize + 1);

    auto* ptr = data.get() + (compSize * dataSize);
    metadataSet[id.id - 1] = {dataSize, ptr};
    ids.push_back(id);

    dataSize++;

    if (parent) {
        parent->onChildInsert(id, ptr);
    }

    return ptr;
}

void ComponentSet::onChildInsert (component::EntityId id, std::byte* ptr) {
    assert(!metadataSet[id.id - 1].data);
    assert(metadataSet[id.id - 1].index == EMPTY_INDEX);

    metadataSet[id.id - 1].data = ptr;
    inheritedSize++;

    if (parent) {
        parent->onChildInsert(id, ptr);
    }
}

bool ComponentSet::deleteComp (EntityId id) {
    assert(metadataSet.size() > id.id - 1);
    if (!metadataSet[id.id - 1].data) {
        return false;
    }

    if (metadataSet[id.id - 1].index == EMPTY_INDEX) {
        for (auto* curr = children; curr; curr = curr->nextChild) {
            if (curr->deleteComp(id)) {
                return true;
            }
        }

        logging::log(LEVEL_ERROR, "Failed to find component for id {} despite it supposedly existing!");
        metadataSet[id.id - 1].data = nullptr;
        return false;
    }

    auto compIndex = metadataSet[id.id - 1].index;
    auto* compPtr = metadataSet[id.id - 1].data;
    runDeletionCallbacks(compPtr, id);
    if (compIndex == dataSize - 1) {
        deleteTypedComp(compPtr);
    } else {
        auto oldCompPtr = data.get() + compSize * (dataSize - 1);
        moveTypedComp(compPtr, oldCompPtr);

        auto movedId = ids[dataSize - 1];
        ids[compIndex] = movedId;
        metadataSet[movedId.id - 1] = {compIndex, compPtr};
    }

    metadataSet[id.id - 1] = {EMPTY_INDEX, nullptr};
    ids.pop_back();
    dataSize--;

    if (parent) {
        parent->onChildDelete(id);
    }

    return true;
}

void ComponentSet::onChildDelete (component::EntityId id) {
    assert(metadataSet[id.id - 1].data);
    assert(inheritedSize > 0);

    metadataSet[id.id - 1].data = nullptr;
    inheritedSize--;

    if (parent) {
        parent->onChildDelete(id);
    }
}

void ComponentSet::guaranteeCapacity (std::size_t capacity) {
    assert(compSize > 0);
    if (dataCapacity >= capacity) {
        return;
    }

    while (dataCapacity < capacity) {
        dataCapacity *= RESIZE_FACTOR;
    }

    auto newData = std::make_unique<std::byte[]>(dataCapacity * compSize);
    moveAllComps(newData.get(), data.get(), dataSize);

    data = std::move(newData);
    ids.reserve(dataCapacity);
    metadataSet.reserve(dataCapacity);
}

void ComponentSet::clear () {
    for (std::size_t i = 0; i < dataSize; i++) {
        runDeletionCallbacks(data.get() + (i * compSize), ids[i]);
        deleteTypedComp(data.get() + (i * compSize));
        auto index = ids[i].id - 1;

        metadataSet[index] = {EMPTY_INDEX, nullptr};

        if (parent) {
            parent->onChildDelete(ids[i]);
        }

        ids[i] = EntityId{};
    }

    dataSize = 0;
}

bool ComponentSet::hasComp (EntityId id) const {
    assert(metadataSet.size() > id.id - 1);
    return metadataSet[id.id - 1].data;
}

bool ComponentSet::setParent (detail::ComponentSet* parentSet) {
    if (parent && parentSet) {
        logging::log(LEVEL_ERROR, "Attempted to set parent for component that already has parent!");
        return false;
    }

    parent = parentSet;
    updateDepth(parent ? parent->hierachyDepth + 1 : 0);

    return true;
}

void ComponentSet::updateDepth (std::size_t newDepth) {
    hierachyDepth = newDepth;
    auto* curr = children;
    while (curr) {
        curr->updateDepth(hierachyDepth + 1);
        curr = curr->nextChild;
    }
}

std::size_t ComponentSet::getHierachyDepth () const {
    return hierachyDepth;
}

ComponentSet* ComponentSet::getParent () const {
    return parent;
}

void ComponentSet::addChild (ComponentSet* child) {
    assert(child);

    if (!children) {
        children = child;
        return;
    }

    auto* curr = children;
    while (curr->nextChild) {
        curr = curr->nextChild;
    }

    curr->nextChild = child;
    child->nextChild = nullptr;
}

void ComponentSet::removeChild (ComponentSet* child) {
    assert(child);

    if (!children) {
        logging::log(LEVEL_ERROR, "Attempted to remove child from component with no children!");
        return;
    } else if (children == child) {
        children = children->nextChild;
        child->nextChild = nullptr;
        return;
    }

    while (children->nextChild) {
        if (children->nextChild == child) {
            children->nextChild = child->nextChild;
            child->nextChild = nullptr;
            return;
        }
    }

    logging::log(LEVEL_ERROR, "Attempted to remove child from component that does not directly parent that child!");
}

std::size_t ComponentSet::size () const {
    return dataSize + inheritedSize;
}