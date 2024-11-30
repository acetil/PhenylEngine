#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#include "util/meta.h"
#include "util/range_utils.h"

namespace phenyl::core::detail {
    class ArchetypeKey {
    private:
        // Sorted vector of component type ids
        std::vector<std::size_t> compIds;

    public:
        ArchetypeKey () = default;
        explicit ArchetypeKey (std::vector<std::size_t> compIds) : compIds{std::move(compIds)} {}

        template <std::forward_iterator It, std::sentinel_for<It> S>
        explicit ArchetypeKey (It first, S last) : ArchetypeKey{std::vector<std::size_t>{first, last}} {}
        template <std::ranges::forward_range R>
        explicit ArchetypeKey (R&& range) : ArchetypeKey{range.begin(), range.end()} {}

        template <typename ...Args>
        static ArchetypeKey Make () {
            std::vector<std::size_t> ids{meta::type_index<std::remove_cvref_t<Args>>()...};
            std::ranges::sort(ids);

            return ArchetypeKey{std::move(ids)};
        }

        [[nodiscard]] bool has (std::size_t id) const noexcept {
            return std::ranges::binary_search(compIds, id);
        }

        template <typename T>
        [[nodiscard]] bool has () const noexcept {
            return has(meta::type_index<std::remove_cvref_t<T>>());
        }

        [[nodiscard]] ArchetypeKey with (std::size_t id) const {
            std::vector<std::size_t> newIds;
            newIds.reserve(compIds.size() + 1);
            // auto it = compIds.begin();
            // while (it != compIds.end() && *it < id) {
            //     newIds.emplace_back(*it);
            //     ++it;
            // }
            //
            // if (it == compIds.end() || *it > id) {
            //     newIds.emplace_back(id);
            // }
            //
            // while (it != compIds.end()) {
            //     newIds.emplace_back(*it);
            //     ++it;
            // }
            std::ranges::set_union(compIds, std::array{id}, std::back_inserter(newIds));
            return ArchetypeKey{std::move(newIds)};
        }

        template <std::forward_iterator It, std::sentinel_for<It> S>
        ArchetypeKey with (It it, S last) {
            std::vector<std::size_t> newIds;
            // auto cIt = compIds.begin();
            // while (cIt != compIds.end() && it != last) {
            //     if (*cIt < *it) {
            //         newIds.emplace_back(*cIt);
            //         ++cIt;
            //     } else if (*it < *cIt) {
            //         newIds.emplace_back(*it);
            //         ++it;
            //     } else {
            //         newIds.emplace_back(*cIt);
            //         ++cIt;
            //         ++it;
            //     }
            // }
            //
            // while (cIt != compIds.end()) {
            //     newIds.emplace_back(*cIt);
            //     ++cIt;
            // }
            //
            // while (it != last) {
            //     newIds.emplace_back(*it);
            //     ++it;
            // }
            std::set_union(compIds.begin(), compIds.end(), it, last, std::back_inserter(newIds));
            return ArchetypeKey{std::move(newIds)};
        }

        template <std::ranges::forward_range R>
        ArchetypeKey with (R&& range) {
            std::vector<std::size_t> newIds;
            std::ranges::set_union(compIds, range, std::back_inserter(newIds));
            return ArchetypeKey{std::move(newIds)};
        }

        template <typename T>
        [[nodiscard]] ArchetypeKey with () const {
            return with(meta::type_index<std::remove_cvref_t<T>>());
        }

        [[nodiscard]] ArchetypeKey without (std::size_t id) const {
            std::vector<std::size_t> newIds;
            newIds.reserve(compIds.size() - 1);
            std::ranges::copy_if(compIds, std::back_inserter(newIds), [id] (auto x) { return x != id; });

            return ArchetypeKey{std::move(newIds)};
        }

        template <typename T>
        [[nodiscard]] ArchetypeKey without () const {
            return without(meta::type_index<std::remove_cvref_t<T>>());
        }

        [[nodiscard]] ArchetypeKey keyUnion (const ArchetypeKey& other) const {
            std::vector<std::size_t> newIds;
            std::ranges::set_union(compIds, other.compIds, std::back_inserter(newIds));

            return ArchetypeKey{std::move(newIds)};
        }

        [[nodiscard]] ArchetypeKey keyIntersection (const ArchetypeKey& other) const {
            std::vector<std::size_t> newIds;
            std::ranges::set_intersection(compIds, other.compIds, std::back_inserter(newIds));

            return ArchetypeKey{std::move(newIds)};
        }

        [[nodiscard]] bool subsetOf (const ArchetypeKey& other) const noexcept {
            return std::ranges::includes(compIds, other.compIds);
        }

        [[nodiscard]] std::ranges::forward_range auto intersectionView (const ArchetypeKey& other) const noexcept {
            return util::RangeIntersection{compIds, other.compIds};
        }

        bool operator== (const ArchetypeKey& other) const {
            return compIds == other.compIds;
        }

        auto begin () const {
            return compIds.begin();
        }

        auto cbegin () const {
            return compIds.cbegin();
        }

        auto end () const {
            return compIds.end();
        }

        auto cend () const {
            return compIds.cend();
        }
    };
}