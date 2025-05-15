#pragma once

#include "util/range_utils.h"
#include "util/type_index.h"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace phenyl::core::detail {
class ArchetypeKey {
public:
    ArchetypeKey () = default;

    explicit ArchetypeKey (std::vector<meta::TypeIndex> compIds) : m_compIds{std::move(compIds)} {}

    template <std::forward_iterator It, std::sentinel_for<It> S>
    explicit ArchetypeKey(It first, S last) : ArchetypeKey{std::vector<meta::TypeIndex>{first, last}} {}

    template <std::ranges::forward_range R>
    explicit ArchetypeKey(R&& range) : ArchetypeKey{range.begin(), range.end()} {}

    template <typename... Args>
    static ArchetypeKey Make () {
        std::vector<meta::TypeIndex> ids{meta::TypeIndex::Get<Args>()...};
        std::ranges::sort(ids);

        return ArchetypeKey{std::move(ids)};
    }

    [[nodiscard]] bool has (meta::TypeIndex id) const noexcept {
        return std::ranges::binary_search(m_compIds, id);
    }

    template <typename T>
    [[nodiscard]] bool has () const noexcept {
        return has(meta::TypeIndex::Get<T>());
    }

    [[nodiscard]] ArchetypeKey with (meta::TypeIndex id) const {
        std::vector<meta::TypeIndex> newIds;
        newIds.reserve(m_compIds.size() + 1);
        std::ranges::set_union(m_compIds, std::array{id}, std::back_inserter(newIds));
        return ArchetypeKey{std::move(newIds)};
    }

    template <std::forward_iterator It, std::sentinel_for<It> S>
    ArchetypeKey with (It it, S last) {
        std::vector<meta::TypeIndex> newIds;
        std::set_union(m_compIds.begin(), m_compIds.end(), it, last, std::back_inserter(newIds));
        return ArchetypeKey{std::move(newIds)};
    }

    template <std::ranges::forward_range R>
    ArchetypeKey with (R&& range) {
        std::vector<meta::TypeIndex> newIds;
        std::ranges::set_union(m_compIds, range, std::back_inserter(newIds));
        return ArchetypeKey{std::move(newIds)};
    }

    template <typename T>
    [[nodiscard]] ArchetypeKey with () const {
        return with(meta::TypeIndex::Get<T>());
    }

    [[nodiscard]] ArchetypeKey without (meta::TypeIndex id) const {
        std::vector<meta::TypeIndex> newIds;
        newIds.reserve(m_compIds.size() - 1);
        std::ranges::copy_if(m_compIds, std::back_inserter(newIds), [id] (auto x) { return x != id; });

        return ArchetypeKey{std::move(newIds)};
    }

    template <typename T>
    [[nodiscard]] ArchetypeKey without () const {
        return without(meta::TypeIndex::Get<T>());
    }

    [[nodiscard]] ArchetypeKey keyUnion (const ArchetypeKey& other) const {
        std::vector<meta::TypeIndex> newIds;
        std::ranges::set_union(m_compIds, other.m_compIds, std::back_inserter(newIds));

        return ArchetypeKey{std::move(newIds)};
    }

    [[nodiscard]] ArchetypeKey keyIntersection (const ArchetypeKey& other) const {
        std::vector<meta::TypeIndex> newIds;
        std::ranges::set_intersection(m_compIds, other.m_compIds, std::back_inserter(newIds));

        return ArchetypeKey{std::move(newIds)};
    }

    [[nodiscard]] bool subsetOf (const ArchetypeKey& other) const noexcept {
        return std::ranges::includes(m_compIds, other.m_compIds);
    }

    [[nodiscard]] std::ranges::forward_range auto intersectionView (const ArchetypeKey& other) const noexcept {
        return util::RangeIntersection{m_compIds, other.m_compIds};
    }

    bool operator== (const ArchetypeKey& other) const {
        return m_compIds == other.m_compIds;
    }

    auto begin () const {
        return m_compIds.begin();
    }

    auto cbegin () const {
        return m_compIds.cbegin();
    }

    auto end () const {
        return m_compIds.end();
    }

    auto cend () const {
        return m_compIds.cend();
    }

private:
    // Sorted vector of component type ids
    std::vector<meta::TypeIndex> m_compIds;
};
} // namespace phenyl::core::detail
