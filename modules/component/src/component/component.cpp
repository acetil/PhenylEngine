#include "component/component.h"
#include "component/prefab.h"

using namespace component;

PrefabBuilder ComponentManager::buildPrefab () {
    return PrefabBuilder{this, prefabs.create()};
}