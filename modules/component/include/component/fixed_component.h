#pragma once

#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include "util/fl_vector.h"
#include "util/game_id.h"
#include "util/iterable.h"
#include "util/meta.h"
#include "util/optional.h"

namespace component {
    namespace detail {
        template <std::random_access_iterator CompVecIt, util::PublicGameIdType IdType>
        class FixedComponentIterator;
    }

    namespace detail {
        template <typename T, util::PublicGameIdType IdType>
        class FixedComponentVector;

        template <std::random_access_iterator CompVecIt, util::PublicGameIdType IdType>
        class FixedComponentIterator {
        private:
            using RefType = decltype(std::get<1>(*std::declval<CompVecIt>()));
            CompVecIt compIt;
            explicit FixedComponentIterator (CompVecIt compIt) : compIt{compIt} {}

            friend class FixedComponentVector<std::remove_cvref_t<RefType>, IdType>;
        public:
            using value_type = std::pair<RefType, IdType>;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag;

            FixedComponentIterator () = default;

            value_type operator* () const {
                return std::pair<RefType, IdType>{compIt->second, compIt->first};
            }
            value_type operator[] (difference_type diff) const {
                return std::pair<RefType, IdType>{compIt[diff].second, compIt[diff].first};
            }

            FixedComponentIterator<CompVecIt, IdType>& operator++ () {
                ++compIt;

                return *this;
            }
            FixedComponentIterator<CompVecIt, IdType> operator++ (int) {
                auto copy = *this;
                ++this;

                return copy;
            }

            FixedComponentIterator<CompVecIt, IdType>& operator-- () {
                --compIt;

                return *this;
            }
            FixedComponentIterator<CompVecIt, IdType> operator-- (int) {
                auto copy = *this;
                --this;

                return copy;
            }

            FixedComponentIterator<CompVecIt, IdType>& operator+= (difference_type diff) {
                compIt += diff;

                return *this;
            }
            FixedComponentIterator<CompVecIt, IdType>& operator-= (difference_type diff) {
                compIt -= diff;
            }

            difference_type operator- (const FixedComponentIterator<CompVecIt, IdType>& other) const {
                return compIt - other.compIt;
            }

            bool operator== (const FixedComponentIterator<CompVecIt, IdType>& other) const {
                return compIt == other.compIt;
            }
            std::strong_ordering operator<=> (const FixedComponentIterator<CompVecIt, IdType>& other) const {
                return compIt <=> other.compIt;
            }

            friend FixedComponentIterator<CompVecIt, IdType> operator+ (const FixedComponentIterator<CompVecIt, IdType>& it, difference_type diff) {
                auto copy = it;
                copy += diff;

                return copy;
            }
            friend FixedComponentIterator<CompVecIt, IdType> operator+ (difference_type diff, const FixedComponentIterator<CompVecIt, IdType>& it) {
                return it + diff;
            }
            friend FixedComponentIterator<CompVecIt, IdType> operator- (const FixedComponentIterator<CompVecIt, IdType>& it, difference_type diff) {
                auto copy = it;
                copy -= diff;

                return copy;
            }
        };

        template <typename T, util::PublicGameIdType IdType>
        class FixedComponentVector {
        private:
            static constexpr std::size_t GenBits = IdType::GenBits;
            std::vector<std::pair<IdType, T>> comps{};
            std::vector<std::size_t> generations{};
            util::FLVector<std::size_t> indexes{};
            std::size_t typeIndex;

            util::Optional<std::size_t> getCompIndex (IdType id) const {
                if (!indexes.present(id.getIndex())) {
                    return util::NullOpt;
                } else if (generations.at(id.getIndex()) != id.getGeneration()) {
                    component::logging::log(LEVEL_ERROR, "Attempted to access element from fixed component vector with incorrect generation! (id: {}, expected: {}, got: {})", id.getIndex(), generations[id.getIndex()], id.getGeneration());
                    return util::NullOpt;
                }

                return {indexes.at(id.getIndex())};
            }

        public:
            using iterator = FixedComponentIterator<typename std::vector<std::pair<IdType, T>>::iterator, IdType>;
            using const_iterator = FixedComponentIterator<typename std::vector<std::pair<IdType, T>>::const_iterator, IdType>;

            explicit FixedComponentVector (std::size_t typeIndex) : typeIndex{typeIndex} {
                auto str = indexes.toString();
                logging::log(LEVEL_DEBUG, "Test: {}", str);
            };

            template <typename ...Args>
            IdType insert (Args&&... args) {
                auto compIndex = comps.size();

                auto idIndex = indexes.push(compIndex);
                assert(idIndex <= generations.size());

                if (idIndex < generations.size()) {
                    generations[idIndex] = (generations[idIndex] + 1) % (1 << GenBits);
                } else {
                    generations.push_back(0);
                }

                IdType id{generations[idIndex], typeIndex, idIndex};

                comps.emplace_back(id, std::forward<Args>(args)...);

                return id;
            }

            util::Optional<T&> get (IdType id) {
                return getCompIndex(id).thenMap([this] (auto index) -> T& {
                    return std::get<1>(comps.at(index));
                });
            }

            util::Optional<const T&> get (IdType id) const {
                return getCompIndex(id).thenMap([this] (auto index) -> const T& {
                    return std::get<1>(comps.at(index));
                });
            }

            const void* getErased (IdType id) const {
                assert(id.getType() == typeIndex);

                return get(id).thenMap([] (auto& comp) { return static_cast<const void*>(&comp); })
                    .orElse(nullptr);
            }

            void remove (IdType id) {
                if (!indexes.present(id.getIndex())) {
                    component::logging::log(LEVEL_ERROR, "Attempted to remove element from fixed component vector with incorrect id! (id: {}, gen: {})", id.getIndex(), id.getGeneration());
                    return;
                } else if (generations.at(id.getIndex()) != id.getGeneration()) {
                    component::logging::log(LEVEL_ERROR, "Attempted to remove element from fixed component vector with incorrect generation! (id: {}, expected: {}, got: {})", id.getIndex(), generations[id.getIndex()], id.getGeneration());
                    return;
                }

                component::logging::log(LEVEL_DEBUG, "Removing (id: {}, gen: {})", id.getIndex(), id.getGeneration());

                auto compIndex = indexes.at(id.getIndex());
                component::logging::log(LEVEL_DEBUG, "Removal index: {}", compIndex);
                if (compIndex != comps.size() - 1) {
                    component::logging::log(LEVEL_DEBUG, "Swapping indexes {} and {}!", compIndex, comps.size() - 1);
                    IdType lastId = std::get<0>(comps[comps.size() - 1]);
                    indexes.at(lastId.getIndex()) = compIndex;
                    component::logging::log(LEVEL_DEBUG, "Id {} is now pointing at {}!", lastId.getIndex(), indexes.at(lastId.getIndex()));
                    comps[compIndex] = std::move(comps[comps.size() - 1]);
                }

                comps.pop_back();
                indexes.remove(id.getIndex());
            }

            iterator begin () {
                return iterator{comps.begin()};
            }

            const_iterator begin () const {
                return cbegin();
            }

            const_iterator cbegin () const {
                return const_iterator{comps.cbegin()};
            }

            iterator end () {
                return iterator{comps.end()};
            }

            const_iterator end () const {
                return cend();
            }

            const_iterator cend () const {
                return const_iterator{comps.cend()};
            }

            util::Iterable<iterator> iterate () {
                return {begin(), end()};
            }

            util::Iterable<iterator> iterate () const {
                return {cbegin(), cend()};
            }
        };
    }

    template <std::size_t GenBits, typename ...CompTypes>
    class FixedComponentManager/* : FixedComponentManagerBase */{
    public:
        static constexpr std::size_t TypeBits = std::bit_ceil(sizeof...(CompTypes));
        using IdType = util::PublicGameId<std::size_t, GenBits, TypeBits>;
    private:
        using TupleType = std::tuple<detail::FixedComponentVector<CompTypes, IdType>...>;
        std::tuple<detail::FixedComponentVector<CompTypes, IdType>...> comps;

        template <std::size_t I = 0>
        void removeInternal (IdType id) {
            if constexpr (I >= std::tuple_size_v<std::tuple<detail::FixedComponentVector<CompTypes, IdType>...>>) {
                logging::log(LEVEL_ERROR, "Attempted to remove fixed component with id of invalid type! (type: {}, id: {}, gen: {})", id.getType(), id.getIndex(), id.getGeneration());
            } else {
                if (id.getType() == I) {
                    std::get<I>(comps).remove(id);
                } else {
                    removeInternal<I + 1>(id);
                }
            }
        }


        template <std::size_t N, typename T, typename ...Args>
        static std::tuple<detail::FixedComponentVector<T, IdType>, detail::FixedComponentVector<Args, IdType>...> constructComps () {
            if constexpr (sizeof...(Args) == 0) {
                return std::tuple<detail::FixedComponentVector<T, IdType>>{detail::FixedComponentVector<T, IdType>{N}};
            } else {
                return std::tuple_cat(std::tuple{detail::FixedComponentVector<T, IdType>{N}}, constructComps<N + 1, Args...>());
            }
        };

        template <std::size_t N, typename T, typename ...Args>
        static std::size_t getTypeIndexInternal (std::size_t type) {
            if (type == N) {
                return meta::type_index<T>();
            } else {
                if constexpr (sizeof...(Args) == 0) {
                    logging::log(LEVEL_ERROR, "Failed to get type index for type {}!", type);
                    return 0;
                } else {
                    return getTypeIndexInternal<N + 1, Args...>(type);
                }
            }
        }

        template <std::size_t N, typename T, typename ...Args>
        const void* getCompErasedInternal (IdType id) const {
            if (id.getType() == N) {
                return std::get<N>(comps).getErased(id);
            } else {
                if constexpr (sizeof...(Args) == 0) {
                    logging::log(LEVEL_ERROR, "Failed to find component for id {}!", id.getValue());
                    return nullptr;
                } else {
                    return getCompErasedInternal<N + 1, Args...>();
                }
            }
        }

    public:
        FixedComponentManager () : comps{constructComps<0, CompTypes...>()} {}

        template <meta::IsInPack<CompTypes...> T, typename ...Args>
        IdType addComponent (Args&&... args) {
            return std::get<meta::pack_type_index<T, CompTypes...>>(comps).insert(std::forward<Args>(args)...);
        }

        template <meta::IsInPack<CompTypes...> T>
        util::Optional<T&> getComponent (IdType id) {
            if (meta::pack_type_index<T, CompTypes...> != id.getIndex()) {
                logging::log(LEVEL_ERROR, "Attempted to access fixed component with id with incorrect type! (id type index: {}, expected index: {})", id.getIndex(), meta::pack_type_index<T, CompTypes...>);
                return util::NullOpt;
            }

            return std::get<meta::pack_type_index<T, CompTypes...>>(comps).get(id);
        }

        template <meta::IsInPack<CompTypes...> T>
        util::Optional<const T&> getComponent (IdType id) const {
            if (meta::pack_type_index<T, CompTypes...> != id.getIndex()) {
                logging::log(LEVEL_ERROR, "Attempted to access fixed component with id with incorrect type! (id type index: {}, expected index: {})", id.getIndex(), meta::pack_type_index<T, CompTypes...>);
                return util::NullOpt;
            }

            return std::get<meta::pack_type_index<T, CompTypes...>>(comps).get(id);
        }

        void remove (IdType id) {
            removeInternal(id);
        }

        template <typename T>
        util::Iterable<typename std::tuple_element_t<meta::pack_type_index<T, CompTypes...>, TupleType>::iterator> iterate () {
            return std::get<meta::pack_type_index<T, CompTypes...>>(comps).iterate();
        }

        template <typename T>
        util::Iterable<typename std::tuple_element_t<meta::pack_type_index<T, CompTypes...>, TupleType>::const_iterator> iterate () const {
            return std::get<meta::pack_type_index<T, CompTypes...>>(comps).iterate();
        }

        std::size_t getTypeIndex (IdType id) const {
            return getTypeIndexInternal<0, CompTypes...>(id.getType());
        }

        const void* getComponentErased (IdType id) const {
            return getCompErasedInternal<0, CompTypes...>(id);
        }
    };
}