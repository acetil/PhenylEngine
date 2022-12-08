#pragma once

#include <concepts>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace util {
    template <typename T>
    concept BackingType = std::unsigned_integral<T>;

    namespace detail {
        struct IdConstructorTag {};

        template<BackingType T, std::size_t GenerationBits, std::size_t TypeBits>
        class GameIdBase {
        private:
            static constexpr std::size_t IndexBits = sizeof(T) * 8 - GenerationBits - TypeBits;
            static_assert(sizeof(T) * 8 > GenerationBits + TypeBits, "Number of bits requested for type and generation are too large!");

            T data = 0;
        public:
            template <std::unsigned_integral V>
            GameIdBase (V generation, V type, V index) {
                assert(generation < (1ul << GenerationBits));
                assert(type < (1ul << TypeBits));
                assert(index < (1ul << IndexBits));
                data = index | (type << IndexBits) | (generation << (IndexBits + TypeBits));
            }

            template <std::unsigned_integral V>
            GameIdBase (V type, V index) {
                assert(type < (1ul << TypeBits));
                assert(index < (1ul << IndexBits));
                data = index | (type << IndexBits);
            }

            template <std::unsigned_integral V>
            GameIdBase (V generation, V index, IdConstructorTag) {
                assert(generation < (1ul << GenerationBits));
                assert(index < (1ul << IndexBits));
                data = index | (generation << (IndexBits + TypeBits));
            }

            template <std::unsigned_integral V>
            GameIdBase (V index) {
                assert(index <= ((1ul << (IndexBits - 1)) | 1));
                data = index;
            }

            GameIdBase() : data{0} {}

            T getIndex () const {
                return data & ((1ul << (IndexBits - 1)) | 1);
            }

            T getType () const {
                return (data >> IndexBits) & ((1ul << TypeBits) - 1);
            }

            T getGeneration () const {
                return (data >> (IndexBits + TypeBits)) & ((1ul << GenerationBits) - 1);
            }

            bool operator== (const GameIdBase<T, GenerationBits, TypeBits>& other) const {
                return data == other.data;
            }
        };
    }

    template <BackingType T, std::size_t _GenerationBits, std::size_t _TypeBits, typename IdOwnerType, typename TagType = IdOwnerType>
    class GameId {
    public:
        static constexpr std::size_t GenBits = _GenerationBits;
        static constexpr std::size_t TypeBits = _TypeBits;
        static constexpr std::size_t IndexBits = sizeof(T) - GenBits - TypeBits;
        using OwnerType = IdOwnerType;
        using Tag = TagType;
        using DataType = T;
    private:
        using BaseType = detail::GameIdBase<T, GenBits, TypeBits>;

        BaseType data;

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits != 0 && TypeBits != 0>>
        GameId (V generation, V type, V index) {
            assert(index != static_cast<T>(-1) & ((1ul << (IndexBits)) - 1));
            data = BaseType{generation, type, index + 1};
        }

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && ((GenBits == 0 && TypeBits != 0) || (GenBits != 0 && TypeBits == 0))>>
        GameId (V genType, V index) {
            assert(index != static_cast<T>(-1) & ((1ul << (IndexBits)) - 1));
            if constexpr (GenBits == 0) {
                data = BaseType{genType, index + 1};
            }

            if constexpr (TypeBits == 0) {
                data = BaseType{genType, index + 1, {}};
            }
        }

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits == 0 && TypeBits == 0>>
        GameId (V index) {
            assert(index != static_cast<T>(-1) & ((1ul << (IndexBits)) - 1));
            data = BaseType{index + 1};
        }

        T getIndex () const {
            return data.getIndex() - 1;
        }

        template <typename U = GameId<T, _GenerationBits, _TypeBits, IdOwnerType, TagType>>
        std::enable_if_t<U::TypeBits != 0, T> getType () {
            return data.getType();
        }

        template <typename U = GameId<T, _GenerationBits, _TypeBits, IdOwnerType, TagType>>
        std::enable_if_t<U::GenBits != 0, T> getGeneration () {
            return data.getGeneration();
        }

    public:
        GameId () : data{} {}

        operator bool () const {
            return data.getIndex() == 0;
        }

        bool operator== (const GameId<T, _GenerationBits, _TypeBits, IdOwnerType, TagType>& other) const {
            return data == other.data;
        }

        friend IdOwnerType;
    };

    template <typename T>
    concept GameIdType = requires {
        {T::GenBits} -> std::same_as<const std::size_t&>;
        {T::TypeBits} -> std::same_as<const std::size_t&>;
        {T::IndexBits} -> std::same_as<const std::size_t&>;
        typename T::DataType;
        typename T::OwnerType;
        typename T::Tag;

        requires std::same_as<T, GameId<typename T::DataType, T::GenBits, T::TypeBits, typename T::OwnerType, typename T::Tag>>;
    };

    template <typename T>
    concept GameIdHasGen = GameIdType<T> && requires {
        requires T::GenBits != 0;
    };

    template <typename T>
    concept GameIdHasType = GameIdType<T> && requires {
        requires T::TypeBits != 0;
    };
}