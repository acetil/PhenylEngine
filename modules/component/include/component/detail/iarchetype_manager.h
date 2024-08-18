#pragma once
#include <memory>
#include <set>

namespace phenyl::component {
    class Archetype;
}

namespace phenyl::component::detail {
    class ArchetypeKey;

    class IArchetypeManager {
    public:
        virtual ~IArchetypeManager() = default;

        virtual Archetype* findArchetype (const ArchetypeKey& key) = 0;
        virtual void updateEntityEntry (EntityId id, Archetype* archetype, std::size_t pos) = 0;

        virtual void onComponentInsert (EntityId id, std::size_t compType, std::byte* ptr) = 0;
        virtual void onComponentRemove (EntityId id, std::size_t compType, std::byte* ptr) = 0;
    };
}
