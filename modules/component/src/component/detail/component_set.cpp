#include "component/detail/component_set.h"
#include "component/detail/signals/signal_handler.h"

using namespace phenyl::component::detail;

ComponentSet::ComponentSet (detail::BasicManager* manager, std::size_t startCapacity, std::size_t compSize) : manager{manager}, ids{}, metadataSet{}, data{compSize != 0 ? std::make_unique<std::byte[]>(startCapacity * compSize) : nullptr},
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
    PHENYL_DASSERT(id.id > 0);
    PHENYL_DASSERT(metadataSet.size() > id.id - 1);

    return metadataSet[id.id - 1].data;
}

bool ComponentSet::canInsert (component::EntityId id) {
    return metadataSet[id.id - 1].empty() && (!parent || parent->canInsert(id));
}

std::byte* ComponentSet::tryInsert (EntityId id) {
    PHENYL_DASSERT(metadataSet.size() > id.id - 1);

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
            PHENYL_DASSERT(dataSize < allSize);

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

        onInsert(id, ptr, ptr);

        return ptr;
    } else {
        auto* ptr = data.get() + (compSize * allSize);
        metadataSet[id.id - 1].fill((std::uint32_t) allSize, nullptr);
        ids.push_back(id);
        allSize++;

        onInsert(id, ptr, nullptr);

        return ptr;
    }
}

void ComponentSet::onInsert (component::EntityId id, std::byte* ptr, std::byte* childPtr) {
    if (parent) {
        parent->onChildInsert(id, childPtr);
    }

    for (auto* i : dependents) {
        i->onDependencyInsert(id);
    }
}

void ComponentSet::onChildInsert (component::EntityId id, std::byte* ptr) {
    PHENYL_DASSERT(metadataSet[id.id - 1].empty());
    PHENYL_DASSERT(!metadataSet[id.id - 1].present());

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
    if (deferring) {
        deferredDeletions.push_back(id);
        return true;
    }

    PHENYL_DASSERT(metadataSet.size() > id.id - 1);
    if (metadataSet[id.id - 1].empty()) {
        return false;
    }

    if (!metadataSet[id.id - 1].present()) {
        for (auto* curr = children; curr; curr = curr->nextChild) {
            if (curr->deleteComp(id)) {
                return true;
            }
        }

        PHENYL_LOGE(COMPONENT_SET_LOGGER, "Failed to find component for id {} despite it supposedly existing!", id.id);
        metadataSet[id.id - 1].clear();
        return false;
    }

    auto compIndex = metadataSet[id.id - 1].index;
    auto* compPtr = data.get() + compSize * compIndex;
    //runDeletionCallbacks(compPtr, id);
    removeHandler->handle(manager, id, OnRemoveUntyped{compPtr});

    if (compIndex == allSize - 1) {
        deleteTypedComp(compPtr);
    } else if (compIndex < dataSize - 1) {
        auto oldCompPtr = data.get() + compSize * (dataSize - 1);
        moveTypedComp(compPtr, oldCompPtr);

        auto movedId = ids[dataSize - 1];
        ids[compIndex] = movedId;
        metadataSet[movedId.id - 1].move(compIndex, compPtr);
        if (parent) {
            parent->onChildRelocate(movedId, compPtr);
        }

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
    PHENYL_DASSERT(!metadataSet[id.id - 1].empty());
    PHENYL_DASSERT(inheritedSize > 0);

    metadataSet[id.id - 1].clear();
    inheritedSize--;

    if (parent) {
        parent->onChildDelete(id);
    }
}

void ComponentSet::guaranteeCapacity (std::size_t capacity) {
    PHENYL_DASSERT(compSize > 0);
    if (dataCapacity >= capacity) {
        return;
    }

    while (dataCapacity < capacity) {
        dataCapacity *= RESIZE_FACTOR;
    }

    auto newData = std::make_unique<std::byte[]>(dataCapacity * compSize);
    moveAllComps(newData.get(), data.get(), allSize);

    data = std::move(newData);
    for (std::size_t i = 0; i < dataSize; i++) {
        metadataSet[ids[i].id - 1].data = data.get() + compSize * i;

        if (parent) {
            parent->onChildRelocate(ids[i], data.get() + compSize * i);
        }
    }
    ids.reserve(dataCapacity);
    metadataSet.reserve(dataCapacity);
}

void ComponentSet::onChildRelocate (EntityId id, std::byte* ptr) {
    if (parent) {
        parent->onChildRelocate(id, ptr);
    }

    metadataSet[id.id - 1].fillChild(ptr);
}

void ComponentSet::clear () {
    for (std::size_t i = 0; i < allSize; i++) {
        deleteTypedComp(data.get() + (i * compSize));
        auto index = ids[i].id - 1;

        metadataSet[index] = Metadata::Empty(dependencySize);

        if (parent) {
            parent->onChildDelete(ids[i]);
        }
    }

    ids.clear();

    dataSize = 0;
    allSize = 0;
}

bool ComponentSet::hasComp (EntityId id) const {
    PHENYL_DASSERT(metadataSet.size() > id.id - 1);
    return !metadataSet[id.id - 1].empty();
}

bool ComponentSet::setParent (ComponentSet* parentSet) {
    if (parent && parentSet) {
        PHENYL_LOGE(COMPONENT_SET_LOGGER, "Attempted to set parentId for component that already has parentId!");
        return false;
    }

    parent = parentSet;
    updateDepth(parent ? parent->hierachyDepth + 1 : 0);

    if (parent) {
        insertHandler->setParent(parent->insertHandler.get());
        statusChangedHandler->setParent(parent->statusChangedHandler.get());
        removeHandler->setParent(parent->removeHandler.get());
    } else {
        insertHandler->setParent(nullptr);
        statusChangedHandler->setParent(nullptr);
        removeHandler->setParent(nullptr);
    }

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

void ComponentSet::addChild (ComponentSet* child) {
    PHENYL_DASSERT(child);

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
    PHENYL_DASSERT(child);

    if (!children) {
        PHENYL_LOGE(COMPONENT_SET_LOGGER, "Attempted to remove child from component with no children!");
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

    PHENYL_LOGE(COMPONENT_SET_LOGGER, "Attempted to remove child from component that does not directly parentId that child!");
}

void ComponentSet::addDependency () {
    dependencySize++;
}

void ComponentSet::addDependent (ComponentSet* dependent) {
    if (!ids.empty()) {
        PHENYL_LOGE(COMPONENT_SET_LOGGER, "Cannot add dependent after entities have been added!");
        return;
    }
    if (std::find(dependents.begin(), dependents.end(), dependent) != dependents.end()) {
        PHENYL_LOGE(COMPONENT_SET_LOGGER, "Dependent has already been added!");
        return;
    }

    dependents.push_back(dependent);
    dependent->addDependency();
}

void ComponentSet::onDependencyInsert (component::EntityId id) {
    PHENYL_DASSERT(metadataSet.size() > id.id - 1);
    metadataSet[id.id - 1].decrementDependencies();

    if (metadataSet[id.id - 1].present() && metadataSet[id.id - 1].active()) {
        activate(id);
    }
}

void ComponentSet::onDependencyRemove (component::EntityId id) {
    PHENYL_DASSERT(metadataSet.size() > id.id - 1);
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

    statusChangedHandler->handle(manager, id, OnStatusChangeUntyped{metadataSet[id.id - 1].data, true});
}

void ComponentSet::deactivate (component::EntityId id) {
    statusChangedHandler->handle(manager, id, OnStatusChangeUntyped{metadataSet[id.id - 1].data, false});

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
        if (parent) {
            parent->onChildRelocate(movedId, oldPtr);
        }
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

void ComponentSet::defer () {
    deferring = true;
}

void ComponentSet::deferEnd () {
    deferring = false;
    for (auto i : deferredDeletions) {
        deleteComp(i);
    }

    deferredDeletions.clear();
    popDeferredInsertions();
}
