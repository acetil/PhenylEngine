#pragma once
#include "util/type_index.h"

#include <memory>
#include <set>

namespace phenyl::core {
class Archetype;
}

namespace phenyl::core::detail {
class ArchetypeKey;

class IArchetypeManager {
public:
    virtual ~IArchetypeManager () = default;

    virtual Archetype* findArchetype (const ArchetypeKey& key) = 0;
    virtual void updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) = 0;

    virtual void onComponentInsert (EntityId id, meta::TypeIndex compType, std::byte* ptr) = 0;
    virtual void onComponentRemove (EntityId id, meta::TypeIndex compType, std::byte* ptr) = 0;
};
} // namespace phenyl::core::detail
