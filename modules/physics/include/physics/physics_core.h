#pragma once

#include <concepts>
#include "util/game_id.h"

namespace physics {
    class IPhysics;
    /*template <typename T>
    struct PhysicsId {
    private:
        std::uint64_t entityId;
        explicit PhysicsId (std::uint64_t entityId) : entityId{entityId} {}
    public:
        using Type = T;

        PhysicsId () : PhysicsId(0) {}

        [[nodiscard]] bool valid () const {
            return entityId;
        }

        bool operator== (const PhysicsId<T>& other) const {
            return entityId == other.entityId;
        }

        friend class IPhysics;
    };*/

    template <typename T, std::size_t GenerationBits, std::size_t TypeBits>
    using PhysicsId = util::GameId<std::size_t, GenerationBits, TypeBits, IPhysics, T>;

    template <typename T>
    concept PhysicsIdType = requires {
        requires util::GameIdType<T>;
        requires std::same_as<std::size_t, typename T::DataType>;
        requires std::same_as<IPhysics, typename T::OwnerType>;
    };
}