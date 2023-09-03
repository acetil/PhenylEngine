#include "component/component.h"
#include "component/detail/children_view.h"
#include "component/prefab.h"

using namespace component;

PrefabBuilder ComponentManager::buildPrefab () {
    return PrefabBuilder{this, prefabs.create()};
}
