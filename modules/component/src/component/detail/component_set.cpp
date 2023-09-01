#include "component/detail/component_set.h"
#include "component/detail/signal_handler.h"

using namespace component::detail;

ComponentSet::ComponentSet (std::size_t startCapacity, std::size_t compSize) : ids{}, metadataSet{}, data{compSize != 0 ? std::make_unique<std::byte[]>(startCapacity * compSize) : nullptr},
                                                                               compSize{compSize}, dataSize{0}, allSize{0}, dataCapacity{startCapacity}, hierachyDepth{0} {
    ids.reserve(startCapacity);
    metadataSet.reserve(startCapacity);
}

ComponentSet::~ComponentSet () = default;

void ComponentSet::guaranteeEntityIndex (std::size_t index) {
    while (metadataSet.size() < index) {
        metadataSet.push_back(Metadata::Empty(dependencySize));
    }
}

std::byte* ComponentSet::getComponentUntyped (EntityId id) const {
    assert(metadataSet.size() > id.id - 1);

    return metadataSet[id.id - 1].data;
}

bool ComponentSet::canInsert (component::EntityId id) {
    return metadataSet[id.id - 1].empty() && (!parent || parent->canInsert(id));
}

std::byte* ComponentSet::tryInsert (EntityId id) {
    assert(metadataSet.size() > id.id - 1);

    if (compSize == 0 || !canInsert(id)) {
        return nullptr;
    }

    guaranteeCapacity(allSize + 1);

    if (metadataSet[id.id - 1].active()) {
        /*auto* ptr = data.get() + (compSize * allSize);
        metadataSet[id.id - 1].fill((std::uint32_t) allSize, ptr);
        ids.push_back(id);
        dataSize++;
        allSize++;

        onInsert(id, ptr);

        return ptr;*/
        std::byte* ptr;
        if (dataSize == allSize) {
            ptr = data.get() + (compSize * allSize);
            metadataSet[id.id - 1].fill((std::uint32_t) allSize, ptr);
            ids.push_back(id);
        } else {
            assert(dataSize < allSize);

            auto* activeEndPtr = data.get() + (compSize * dataSize);
            auto* dataEndPtr = data.get() + (compSize * allSize);
            moveTypedComp(dataEndPtr, activeEndPtr);
            ptr = activeEndPtr;

            auto oldId = ids[dataSize];
            metadataSet[oldId.id - 1].fill((std::uint32_t) allSize, nullptr);
            ids.push_back(oldId);

            ids[dataSize] = id;
            metadataSet[id.id - 1].fill((std::uint32_t) dataSize, ptr);
        }

        allSize++;
        dataSize++;

        onInsert(id, ptr);

        return ptr;
    } else {
        auto* ptr = data.get() + (compSize * allSize);
        metadataSet[id.id - 1].fill((std::uint32_t) allSize, nullptr);
        ids.push_back(id);
        allSize++;

        onInsert(id, nullptr);

        return ptr;
    }
}

void ComponentSet::onInsert (component::EntityId id, std::byte* ptr) {
    if (parent) {
        parent->onChildInsert(id, ptr);
    }

    for (auto* i : dependents) {
        i->onDependencyInsert(id);
    }
}

void ComponentSet::onChildInsert (component::EntityId id, std::byte* ptr) {
    assert(metadataSet[id.id - 1].empty());
    assert(!metadataSet[id.id - 1].present());

    metadataSet[id.id - 1].fillChild(ptr);
    inheritedSize++;

    for (auto* i : dependents) {
        i->onDependencyInsert(id);
    }

    if (parent) {
        parent->onChildInsert(id, ptr);
    }
}

bool ComponentSet::deleteComp (EntityId id) {
    assert(metadataSet.size() > id.id - 1);
    if (metadataSet[id.id - 1].empty()) {
        return false;
    }

    if (!metadataSet[id.id - 1].present()) {
        for (auto* curr = children; curr; curr = curr->nextChild) {
            if (curr->deleteComp(id)) {
                return true;
            }
        }

        logging::log(LEVEL_ERROR, "Failed to find component for id {} despite it supposedly existing!");
        metadataSet[id.id - 1].clear();
        return false;
    }

    auto compIndex = metadataSet[id.id - 1].index;
    auto* compPtr = data.get() + compSize * compIndex;
    runDeletionCallbacks(compPtr, id);
    if (compIndex == allSize - 1) {
        deleteTypedComp(compPtr);
    } else if (compIndex < dataSize - 1) {
        auto oldCompPtr = data.get() + compSize * (dataSize - 1);
        moveTypedComp(compPtr, oldCompPtr);

        auto movedId = ids[dataSize - 1];
        ids[compIndex] = movedId;
        metadataSet[movedId.id - 1].move(compIndex, compPtr);

        if (dataSize < allSize) {
            auto oldEndPtr = data.get() + compSize * (allSize - 1);
            moveTypedComp(oldCompPtr, oldEndPtr);

            auto endMovedId = ids[allSize - 1];
            ids[dataSize - 1] = endMovedId;
            metadataSet[endMovedId.id - 1].move(dataSize - 1, nullptr);
        }

    } else {
        auto oldCompPtr = data.get() + compSize * (allSize - 1);
        moveTypedComp(compPtr, oldCompPtr);

        auto movedId = ids[allSize - 1];
        ids[compIndex] = movedId;
        metadataSet[movedId.id - 1].move(compIndex, nullptr);
    }

    if (metadataSet[id.id - 1].active()) {
        dataSize--;
    }
    metadataSet[id.id - 1].clear();
    ids.pop_back();
    allSize--;

    onRemove(id);

    return true;
}

void ComponentSet::onRemove (component::EntityId id) {
    if (parent) {
        parent->onChildDelete(id);
    }

    for (auto* i : dependents) {
        i->onDependencyRemove(id);
    }
}

void ComponentSet::onChildDelete (component::EntityId id) {
    assert(!metadataSet[id.id - 1].empty());
    assert(inheritedSize > 0);

    metadataSet[id.id - 1].clear();
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
    moveAllComps(newData.get(), data.get(), allSize);

    data = std::move(newData);
    ids.reserve(dataCapacity);
    metadataSet.reserve(dataCapacity);
}

void ComponentSet::clear () {
    for (std::size_t i = 0; i < allSize; i++) {
        runDeletionCallbacks(data.get() + (i * compSize), ids[i]);
        deleteTypedComp(data.get() + (i * compSize));
        auto index = ids[i].id - 1;

        metadataSet[index].clear();

        if (parent) {
            parent->onChildDelete(ids[i]);
        }

        ids[i] = EntityId{};
    }

    dataSize = 0;
    allSize = 0;
}

bool ComponentSet::hasComp (EntityId id) const {
    assert(metadataSet.size() > id.id - 1);
    return !metadataSet[id.id - 1].empty();
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

void ComponentSet::addDependency () {
    dependencySize++;
}

void ComponentSet::addDependent (ComponentSet* dependent) {
    if (!ids.empty()) {
        logging::log(LEVEL_ERROR, "Cannot add dependent after entities have been added!");
        return;
    }
    if (std::find(dependents.begin(), dependents.end(), dependent) != dependents.end()) {
        logging::log(LEVEL_ERROR, "Dependent has already been added!");
        return;
    }

    dependents.push_back(dependent);
    dependent->addDependency();
}

void ComponentSet::onDependencyInsert (component::EntityId id) {
    assert(metadataSet.size() > id.id - 1);
    metadataSet[id.id - 1].decrementDependencies();

    if (metadataSet[id.id - 1].present() && metadataSet[id.id - 1].active()) {
        activate(id);
    }
}

void ComponentSet::onDependencyRemove (component::EntityId id) {
    assert(metadataSet.size() > id.id - 1);
    bool active = metadataSet[id.id - 1].active();
    metadataSet[id.id - 1].incrementDepenencies();

    if (metadataSet[id.id - 1].present() && active) {
        deactivate(id);
    }
}

void ComponentSet::activate (component::EntityId id) {
    if (metadataSet[id.id - 1].index == dataSize) {
        metadataSet[id.id - 1].move(dataSize, data.get() + (compSize * dataSize));
    } else {
        auto oldIndex = metadataSet[id.id - 1].index;
        auto* ptr = data.get() + compSize * dataSize;
        auto* oldPtr = data.get() + compSize * oldIndex;
        swapTypedComp(ptr, oldPtr);

        auto movedId = ids[dataSize];
        ids[dataSize] = id;
        ids[oldIndex] = movedId;

        metadataSet[movedId.id - 1].move(oldIndex, nullptr);
        metadataSet[id.id - 1].move(dataSize, ptr);
    }

    dataSize++;

    if (parent) {
        parent->onChildUpdate(id, metadataSet[id.id - 1].data);
    }
}

void ComponentSet::deactivate (component::EntityId id) {
    if (metadataSet[id.id - 1].index == dataSize - 1) {
        metadataSet[id.id - 1].move(dataSize - 1, nullptr);
    } else {
        auto oldIndex = metadataSet[id.id - 1].index;
        auto* ptr = data.get() + compSize * (dataSize - 1);
        auto* oldPtr = data.get() + compSize * oldIndex;
        swapTypedComp(ptr, oldPtr);

        auto movedId = ids[dataSize - 1];
        ids[dataSize - 1] = id;
        ids[oldIndex] = movedId;

        metadataSet[movedId.id - 1].move(oldIndex, oldPtr);
        metadataSet[id.id - 1].move(dataSize - 1, nullptr);
    }

    dataSize--;

    if (parent) {
        parent->onChildUpdate(id, nullptr);
    }
}


void ComponentSet::onChildUpdate (component::EntityId id, std::byte* ptr) {
    if (parent) {
        parent->onChildUpdate(id, ptr);
    }

    metadataSet[id.id - 1].fillChild(ptr);
}
