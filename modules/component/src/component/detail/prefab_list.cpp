#include "component/detail/prefab_list.h"
#include "component/component.h"

void component::detail::PrefabList::instantiate (component::Entity entity, std::size_t prefabId) {
    assert(valid(prefabId));

    auto& entry = getPrefabEntry(prefabId);
    for (auto& [set, compId] : entry.comps) {
        set->insertPrefab(entity.id(), compId);
    }

    if (entry.inherits) {
        instantiate(entity, entry.inherits);
    }

    for (const auto& i : entry.children) {
        //auto childId = manager->create(id);
        auto child = entity.createChild();
        instantiate(child, i);
    }
}
