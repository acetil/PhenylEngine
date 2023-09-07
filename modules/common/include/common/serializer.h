#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

#include "graphics/maths_headers.h"
#include "serializer_intrusive.h"

namespace common {
    class SerializeVisitor;
    class DeserializeVisitor;

    template <typename C, typename T>
    concept CustomSerializerType = requires (T& t, SerializeVisitor& sv, DeserializeVisitor& dv) {
        { C::Name } -> std::convertible_to<const std::string&>;
        { C::Factory() } -> std::same_as<T>;
        { C::Accept(sv, (const T&)t) } -> std::same_as<bool>;
        { C::Accept(dv, t) } -> std::same_as<bool>;
    };

    template <typename T>
    concept CustomSerializable = requires (T t) {
        { phenyl_serialization_obj(&t) } -> CustomSerializerType<T>;
    };

    namespace detail {
        template <typename T>
        struct IsVector {
            static constexpr bool val = false;
        };

        template <typename T>
        struct IsVector<std::vector<T>> {
            static constexpr bool val = true;
        };
    }

    template <typename T>
    concept DefaultSerializable = std::same_as<T, bool> || std::same_as<T, std::string> || detail::IsVector<T>::val
            || std::same_as<T, std::int8_t> || std::same_as<T, std::int16_t> || std::same_as<T, std::int32_t> || std::same_as<T, std::int64_t>
            || std::same_as<T, std::uint8_t> || std::same_as<T, std::uint16_t> || std::same_as<T, std::uint32_t> || std::same_as<T, std::uint64_t>
            || std::same_as<T, float> || std::same_as<T, double>;

    template <typename T>
    concept Serializable = DefaultSerializable<T> || CustomSerializable<T>;

    template <CustomSerializable T>
    using CustomSerializer = decltype(phenyl_serialization_obj((T*) nullptr));

    namespace detail {
        template <CustomSerializable T>
        T SerializerFactory () {
            return CustomSerializer<T>::Factory();
        }

        template <DefaultSerializable T>
        T SerializerFactory () {
            return T{};
        }
    }

    class SerializeVisitor {
    protected:
        virtual bool pushMember (const std::string& memberName) = 0;
        virtual void popMember () = 0;

        virtual bool pushList () = 0;
        virtual bool listNext () = 0;
        virtual void popList () = 0;

        virtual bool pushArray (std::size_t size) = 0;
        virtual bool arrayNext () = 0;
        virtual void popArray () = 0;
    public:
        static constexpr bool IsSerializer = true;
        static constexpr bool IsDeserializer = false;

        virtual ~SerializeVisitor() = default;

        virtual bool pushObject () = 0;
        virtual void popObject () = 0;

        template <Serializable T>
        bool visitMember (const T& member, const std::string& memberName) {
            if (!pushMember(memberName)) {
                return false;
            }
            auto res = visit(member);
            popMember();
            return res;
        }

        template <CustomSerializable T>
        bool visit (const T& val) {
            bool res = CustomSerializer<T>::Accept(*this, val);

            return res;
        }

        template <Serializable T>
        bool visit (const std::vector<T>& vec) {
            if (!pushList()) {
                return false;
            }
            for (auto& i : vec) {
                if (!visit(i)) {
                    popList();
                    return false;
                }
                if (!listNext()) {
                    return false;
                }
            }
            popList();

            return true;
        }

        template <std::size_t N, Serializable T>
        bool visitArray (const T* vals) {
            if (!pushArray(N)) {
                return false;
            }

            for (std::size_t i = 0; i < N; i++) {
                if (!visit(vals[i])) {
                    popArray();
                    return false;
                }

                if (i < N - 1 && !arrayNext()) {
                    return false;
                }
            }
            popArray();

            return true;
        }

        virtual bool visit (bool val) = 0;
        virtual bool visit (std::string val) = 0;

        virtual bool visit (std::int8_t val) = 0;
        virtual bool visit (std::uint8_t val) {
            return visit(static_cast<std::int8_t>(val));
        }

        virtual bool visit (std::int16_t val) = 0;
        virtual bool visit (std::uint16_t val) {
            return visit(static_cast<std::int16_t>(val));
        }

        virtual bool visit (std::int32_t val) = 0;
        virtual bool visit (std::uint32_t val) {
            return visit(static_cast<std::int32_t>(val));
        }

        virtual bool visit (std::int64_t val) = 0;
        virtual bool visit (std::uint64_t val) {
            return visit(static_cast<std::int64_t>(val));
        }

        virtual bool visit (float val) = 0;
        virtual bool visit (double val) {
            return visit(static_cast<float>(val));
        }
    };

    class DeserializeVisitor {
    protected:
        virtual bool pushMember (const std::string& memberName) = 0;
        virtual void popMember () = 0;

        virtual bool pushList () = 0;
        virtual bool listNext () = 0;
        virtual void popList () = 0;

        virtual bool pushArray (std::size_t size) = 0;
        virtual bool arrayNext () = 0;
        virtual void popArray () = 0;
    public:
        static constexpr bool IsSerializer = false;
        static constexpr bool IsDeserializer = true;
        virtual ~DeserializeVisitor() = default;

        virtual bool pushObject () = 0;
        virtual void popObject () = 0;

        template <Serializable T>
        bool visitMember (T& member, const std::string& memberName) {
            if (!pushMember(memberName)) {
                return false;
            }
            auto res = visit(member);
            popMember();
            return res;
        }

        template <CustomSerializable T>
        bool visit (T& val) {
            bool res = CustomSerializer<T>::Accept(*this, val);

            return res;
        }

        template <Serializable T>
        bool visit (std::vector<T>& vec) {
            if (!pushList()) {
                return false;
            }

            while (listNext()) {
                T t = detail::SerializerFactory<T>();
                if (!visit(t)) {
                    popList();
                    return false;
                }
            }

            popList();

            return true;
        }

        template <std::size_t N, Serializable T>
        bool visitArray (T* vals) {
            if (!pushArray(N)) {
                return false;
            }

            for (std::size_t i = 0; i < N; i++) {
                if (!visit(vals[i])) {
                    popArray();
                    return false;
                }

                if (i < N - 1 && !arrayNext()) {
                    return false;
                }
            }
            popArray();

            return true;
        }

        virtual bool visit (bool& val) = 0;
        virtual bool visit (std::string& val) = 0;

        virtual bool visit (std::int8_t& val) = 0;
        virtual bool visit (std::uint8_t& val) {
            return visit(reinterpret_cast<std::int8_t&>(val));
        }

        virtual bool visit (std::int16_t& val) = 0;
        virtual bool visit (std::uint16_t& val) {
            return visit(reinterpret_cast<std::int16_t&>(val));
        }

        virtual bool visit (std::int32_t& val) = 0;
        virtual bool visit (std::uint32_t& val) {
            return visit(reinterpret_cast<std::int32_t&>(val));
        }

        virtual bool visit (std::int64_t& val) = 0;
        virtual bool visit (std::uint64_t& val) {
            return visit(reinterpret_cast<std::int64_t&>(val));
        }

        virtual bool visit (float& val) = 0;
        virtual bool visit (double& val) {
            return visit(reinterpret_cast<float&>(val));
        }
    };

    namespace detail {
        template <typename T>
        concept IsSerializeVisitor = std::derived_from<T, SerializeVisitor>;

        template <typename T>
        concept IsDeserializeVisitor = std::derived_from<T, DeserializeVisitor>;
    }
}

#define PHENYL_SERIALIZE_NAMED_INT(T, name, details, preVisit, postVisit)                         \
    struct phenyl_##T##_SerializerType {                                 \
        static constexpr const char* Name = name;                        \
                                                                         \
        static T Factory () {                                    \
            return T{};                                                  \
        }                                                                \
        template <common::detail::IsSerializeVisitor V>                                                                 \
        static bool Accept (V& visitor, const T& val) {   \
            constexpr bool IN_SERIALIZE = V::IsSerializer;                          \
            constexpr bool IN_DESERIALIZE = V::IsDeserializer;                       \
            using Type = T;                                              \
                                                                         \
            preVisit                                                                          \
                                        \
            details                                                                           \
            \
            postVisit\
            return true;                                                 \
        }                                                                \
        template <common::detail::IsDeserializeVisitor V>                                                           \
        static bool Accept (V& visitor, T& val) { \
            constexpr bool IN_SERIALIZE = V::IsSerializer;                         \
            constexpr bool IN_DESERIALIZE = V::IsDeserializer;                        \
            using Type = T;                                              \
                                                                                                  \
            preVisit                                                                                      \
            \
            details                                                                               \
                                                                                                  \
            postVisit\
                                                                         \
            return true;                                                 \
        }                                                                \
    };                                                                   \
                                                                         \
                                                                         \
    phenyl_##T##_SerializerType phenyl_serialization_obj (T*);\

#define PHENYL_SERIALIZE_NAMED(T, name, details) PHENYL_SERIALIZE_NAMED_INT(T, name, details, { \
        if constexpr (IN_SERIALIZE || IN_DESERIALIZE) {                                                                                        \
            if (!visitor.pushObject()) {                                                            \
                return false;                                                                                     \
            }                                                                                       \
        }                                                                                               \
    }, {                                                                                        \
        if constexpr (IN_SERIALIZE || IN_DESERIALIZE) {                                                                                        \
            visitor.popObject();                                                                                       \
        }                                                                                             \
    })

#define PHENYL_SERIALIZE(T, details) PHENYL_SERIALIZE_NAMED(T, #T, details)

#define PHENYL_SERIALIZE_ARRAY_FUNC_NAMED(T, name, func, size) PHENYL_SERIALIZE_NAMED_INT(T, name, { \
        if constexpr (IN_SERIALIZE || IN_DESERIALIZE) {                                                                    \
            visitor.template visitArray<size>((func)(val));\
        }                                                                                                     \
    }, {}, {})

#define PHENYL_SERIALIZE_ARRAY_NAMED(T, name, size) PHENYL_SERIALIZE_ARRAY_FUNC_NAMED(T, name, , size)
#define PHENYL_SERIALIZE_ARRAY_FUNC(T, func, size) PHENYL_SERIALIZE_ARRAY_FUNC_NAMED(T, #T, func, size)

#define PHENYL_MEMBER_NAMED(member, name) \
    do {                                  \
        if constexpr (IN_SERIALIZE || IN_DESERIALIZE) {                                  \
            if (!visitor.visitMember(val.member, name)) {              \
                return false;             \
            }\
        }                                 \
    } while (0)

#define PHENYL_MEMBER(member) PHENYL_MEMBER_NAMED(member, #member)

#define PHENYL_MEMBER_METHOD(name, getMethod, setMethod)  \
    do {                                                  \
        if constexpr (V::IsSerializer) {                     \
            if (!visitor.visitMember(val.getMethod(), name)) { \
                return false;                                              \
            }   \
        }                                                 \
                                                          \
        if constexpr (V::IsDeserializer) {                \
            using ValType = std::remove_cvref_t<decltype(val.getMethod())>; \
            ValType newVal = ::common::detail::SerializerFactory<ValType>();   \
            if (!visitor.visitMember(newVal, name)) {     \
                return false;                                              \
            }                                             \
            const_cast<Type&>(val).setMethod(std::move(newVal));                                              \
            \
        }                                         \
    } while (0)                                           \

#define PHENYL_INHERITS_NAMED(Base, name)  \
    do {                                   \
        static_assert(std::derived_from<Type, Base>);\
        if constexpr (IN_SERIALIZE) {      \
            if (!visitor.visitMember(static_cast<const Base&>(val), name)) { \
                return false;                               \
            }                                   \
        }                                  \
                                           \
        if constexpr (IN_DESERIALIZE) {       \
            if (!visitor.visitMember(static_cast<Base&>(const_cast<Type&>(val)), name)) {       \
                return false;                               \
            }                                   \
        }\
    } while (0)

#define PHENYL_INHERITS(Base) PHENYL_INHERITS_NAMED(Base, (::common::CustomSerializer<Base>::Name))

#define PHENYL_MEMBER_ARRAY_NAMED(member, name, size) \
    do { \
        if constexpr (IN_SERIALIZE) {                 \
            if (!visitor.visitArray<size>(val.member, name))                                              \
        }                                                       \
    }

