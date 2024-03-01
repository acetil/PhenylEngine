#pragma once

#include <cstddef>

#include "component_set.h"
#include "loggers.h"
#include "util/fl_vector.h"

namespace phenyl::component::detail {
    struct PrefabEntry {
        std::size_t inherits{};
        std::size_t refCount{1};

        std::vector<std::size_t> children{};
        std::vector<std::pair<ComponentSet*, std::size_t>> comps{};
    };

    class PrefabList {
    private:
        util::FLVector<PrefabEntry> entries;

        PrefabEntry& getPrefabEntry (std::size_t id) {
            assert(valid(id));
            return entries[id - 1];
        }

        void deleteEntry (std::size_t id) {
            assert(valid(id));
            auto& entry = getPrefabEntry(id);

            for (auto& [set, compId] : entry.comps) {
                set->deletePrefab(compId);
            }

            decRefCount(entry.inherits);

            for (auto i : entry.children) {
                decRefCount(i);
            }
        }

    public:
        std::size_t create () {
            return entries.push(PrefabEntry{}) + 1;
        }

        void incRefCount (std::size_t id) {
            assert(valid(id));
            getPrefabEntry(id).refCount++;
        }

        void decRefCount (std::size_t id) {
            if (!valid(id)) {
                PHENYL_LOGE(PREFAB_LOGGER, "Attempted to decrement ref count of invalid prefab {}!", id);
                return;
            }

            if (--getPrefabEntry(id).refCount) {
                deleteEntry(id);
            }
        }

        void setInherits (std::size_t id, std::size_t inherits) {
            PHENYL_DASSERT(valid(id));
            PHENYL_DASSERT(valid(inherits));
            if (getPrefabEntry(id).inherits) {
                PHENYL_LOGE(PREFAB_LOGGER, "Prefab {} already has inherits set (set to {}, attempted to set to {})!", id, getPrefabEntry(id).inherits, inherits);
                return;
            }

            getPrefabEntry(id).inherits = inherits;
            incRefCount(inherits);
        }

        void addComponent (std::size_t id, ComponentSet* set, std::size_t compId) {
            PHENYL_DASSERT(valid(id));
            PHENYL_DASSERT(set);
            getPrefabEntry(id).comps.emplace_back(set, compId);
        }

        void addChild (std::size_t id, std::size_t child) {
            PHENYL_DASSERT(valid(id));
            PHENYL_DASSERT(valid(child));
            getPrefabEntry(id).children.push_back(child);
            incRefCount(child);
        }

        bool valid (std::size_t id) const {
            return id && entries.present(id - 1);
        }

        void instantiate (Entity entity, std::size_t prefabId);
        void clear () {
            for (auto& i : entries) {
                for (auto& [set, compId] : i.comps) {
                    set->deletePrefab(compId);
                }
            }

            entries.clear();
        }
    };
}