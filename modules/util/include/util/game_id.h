#pragma once

#include <concepts>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace phenyl::util {
    template <typename T>
    concept BackingType = std::unsigned_integral<T>;

    namespace detail {
        struct IdConstructorTag {};

        template <BackingType T, std::size_t Bits>
        static constexpr std::size_t safeMask () {
            static_assert(Bits <= sizeof(T) * 8);

            if constexpr (Bits == sizeof(T) * 8) {
                return static_cast<T>(-1);
            } else {
                return (static_cast<T>(1) << Bits) - 1;
            }
        }

        template<BackingType T, std::size_t GenerationBits, std::size_t TypeBits>
        class GameIdData {
        private:
            static constexpr std::size_t IndexBits = sizeof(T) * 8 - GenerationBits - TypeBits;
            static_assert(sizeof(T) * 8 > GenerationBits + TypeBits, "Number of bits requested for type and generation are too large!");

            T data = 0;
        public:
            template <std::unsigned_integral V>
            GameIdData (V generation, V type, V index) {
                assert(generation < (1ul << GenerationBits));
                assert(type < (1ul << TypeBits));
                assert(index < (1ul << IndexBits));
                data = index | (type << IndexBits) | (generation << (IndexBits + TypeBits));
            }

            template <std::unsigned_integral V>
            GameIdData (V type, V index) {
                assert(type < (1ul << TypeBits));
                assert(index < (1ul << IndexBits));
                data = index | (type << IndexBits);
            }

            template <std::unsigned_integral V>
            GameIdData (V generation, V index, IdConstructorTag) {
                assert(generation < (1ul << GenerationBits));
                assert(index < (1ul << IndexBits));
                data = index | (generation << (IndexBits + TypeBits));
            }

            template <std::unsigned_integral V>
            GameIdData (V index) {
                assert(index <= ((1ul << (IndexBits - 1)) | 1));
                data = index;
            }

            GameIdData() : data{0} {}

            T getIndex () const {
                return data & safeMask<T, IndexBits>();
            }

            T getType () const {
                return (data >> IndexBits) & ((1ul << TypeBits) - 1);
            }

            T getGeneration () const {
                return (data >> (IndexBits + TypeBits)) & ((1ul << GenerationBits) - 1);
            }

            T getValue () const {
                return data;
            }

            bool operator== (const GameIdData<T, GenerationBits, TypeBits>& other) const {
                return data == other.data;
            }
        };
    }

    template <BackingType T, std::size_t GenerationBits, std::size_t _TypeBits, typename IdOwnerType, typename TagType>
    class GameId;

    template <BackingType T, std::size_t GenerationBits, std::size_t TypeBits_>
    class PublicGameId {
    public:
        static constexpr std::size_t GenBits = GenerationBits;
        static constexpr std::size_t TypeBits = TypeBits_;
        static constexpr std::size_t IndexBits = sizeof(T) * 8 - GenBits - TypeBits;
        using DataType = T;
    private:
        using BaseType = detail::GameIdData<T, GenBits, TypeBits>;

        BaseType data;
    public:
        PublicGameId () : data{} {}

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits != 0 && TypeBits != 0>>
        explicit PublicGameId (V generation, V type, V index) {
            assert(index != (detail::safeMask<T, IndexBits>()));
            data = BaseType{generation, type, index + 1};
        }

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && ((GenBits == 0 && TypeBits != 0) || (GenBits != 0 && TypeBits == 0))>>
        explicit PublicGameId (V genType, V index) {
            assert(index != (detail::safeMask<T, IndexBits>()));
            if constexpr (GenBits == 0) {
                data = BaseType{genType, index + 1};
            }

            if constexpr (TypeBits == 0) {
                data = BaseType{genType, index + 1, {}};
            }
        }

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits == 0 && TypeBits == 0>>
        explicit PublicGameId (V index) {
            assert(index != (detail::safeMask<T, IndexBits>()));
            data = BaseType{index + 1};
        }

        T getIndex () const {
            return data.getIndex() - 1;
        }

        template <typename U = PublicGameId<T, GenerationBits, TypeBits>>
        std::enable_if_t<U::TypeBits != 0, T> getType () {
            return data.getType();
        }

        template <typename U = PublicGameId<T, GenerationBits, TypeBits>>
        std::enable_if_t<U::GenBits != 0, T> getGeneration () {
            return data.getGeneration();
        }

        operator bool () const {
            return data.getIndex() != 0;
        }

        bool operator== (const PublicGameId<T, GenerationBits, TypeBits>& other) const {
            return data == other.data;
        }

        DataType getValue () const {
            return data.getValue();
        }

        template <typename IdType>
        IdType convert () const {
            return IdType{*this};
        }
    };


    template <BackingType T, std::size_t GenerationBits, std::size_t _TypeBits, typename IdOwnerType, typename TagType = IdOwnerType>
    class GameId {
    public:
        static constexpr std::size_t GenBits = GenerationBits;
        static constexpr std::size_t TypeBits = _TypeBits;
        static constexpr std::size_t IndexBits = sizeof(T) * 8 - GenBits - TypeBits;
        using OwnerType = IdOwnerType;
        using Tag = TagType;
        using DataType = T;
    private:
        using BaseType = PublicGameId<T, GenBits, TypeBits>;

        BaseType data;

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits != 0 && TypeBits != 0>>
        GameId (V generation, V type, V index) : data{generation, type, index} {}

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && ((GenBits == 0 && TypeBits != 0) || (GenBits != 0 && TypeBits == 0))>>
        GameId (V genType, V index) : data{genType, index} {}

        template <std::unsigned_integral V, typename = std::enable_if_t<std::is_integral_v<V> && GenBits == 0 && TypeBits == 0>>
        GameId (V index) : data{index} {}

        GameId (BaseType base) : data{base} {}

        T getIndex () const {
            return data.getIndex();
        }

        template <typename U = GameId<T, GenerationBits, TypeBits, IdOwnerType, TagType>>
        std::enable_if_t<U::TypeBits != 0, T> getType () {
            return data.getType();
        }

        template <typename U = GameId<T, GenerationBits, TypeBits, IdOwnerType, TagType>>
        std::enable_if_t<U::GenBits != 0, T> getGeneration () {
            return data.getGeneration();
        }

        BaseType convert () const {
            return data;
        }

    public:
        GameId () : data{} {}

        GameId (const GameId&) = default;
        GameId& operator= (const GameId&) = default;

        GameId (GameId&&) noexcept = default;
        GameId& operator= (GameId&&) noexcept = default;

        operator bool () const {
            return data;
        }

        bool operator== (const GameId<T, GenerationBits, TypeBits, IdOwnerType, TagType>& other) const {
            return data == other.data;
        }

        DataType getValue () const {
            return data.getValue();
        }

        friend IdOwnerType;
        friend PublicGameId<T, GenBits, TypeBits>;
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
    concept PublicGameIdType = requires {
        {T::GenBits} -> std::same_as<const std::size_t&>;
        {T::TypeBits} -> std::same_as<const std::size_t&>;
        {T::IndexBits} -> std::same_as<const std::size_t&>;
        typename T::DataType;

        requires std::same_as<T, PublicGameId<typename T::DataType, T::GenBits, T::TypeBits>>;
    };

    template <typename T, typename Owner>
    concept OwnedGameIdType = GameIdType<T> && requires {
        requires std::same_as<typename T::OwnerType, Owner>;
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