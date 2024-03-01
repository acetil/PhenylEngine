#pragma once

#include <concepts>
#include <string>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include "graphics/maths_headers.h"
#include "serializer_intrusive.h"
#include "detail/loggers.h"
#include "util/optional.h"

namespace phenyl::common {
    class JsonSerializer;
    class JsonDeserializer;

    template <typename C, typename T>
    concept CustomSerializerType = requires (T& t, JsonSerializer& sv, const JsonDeserializer& dv) {
        { C::Name } -> std::convertible_to<const std::string&>;
        { C::Factory() } -> std::same_as<T>;
        { C::Accept(sv, (const T&)t) } -> std::same_as<bool>;
        { C::Accept(dv, t) } -> std::same_as<bool>;
    };

    namespace detail {
        template <typename T>
        concept SerializableFloat = std::same_as<std::remove_cvref_t<T>, float> || std::same_as<std::remove_cvref_t<T>, double>;

        template <typename T>
        concept SerializableInt = std::same_as<std::remove_cvref_t<T>, std::int8_t> || std::same_as<std::remove_cvref_t<T>, std::int16_t> || std::same_as<std::remove_cvref_t<T>, std::int32_t> || std::same_as<std::remove_cvref_t<T>, std::int64_t>;

        template <typename T>
        concept SerializableUInt = std::same_as<std::remove_cvref_t<T>, std::uint8_t> || std::same_as<std::remove_cvref_t<T>, std::uint16_t> || std::same_as<std::remove_cvref_t<T>, std::uint32_t> || std::same_as<std::remove_cvref_t<T>, std::uint64_t>;
    }

    template <typename T>
    concept DefaultSerializable = std::same_as<std::remove_cvref_t<T>, bool> || std::same_as<std::remove_cvref_t<T>, std::string> || detail::SerializableInt<std::remove_cvref_t<T>> || detail::SerializableUInt<std::remove_cvref_t<T>> || detail::SerializableFloat<std::remove_cvref_t<T>>;


    template <typename T>
    concept CustomSerializable = requires (std::remove_cvref_t<T> t) {
        { phenyl_serialization_obj(&t) } -> CustomSerializerType<std::remove_cvref_t<T>>;
    };

    namespace detail {
        template <typename T>
        struct ContainerSerializable {
        public:
            static constexpr bool val = false;
        };

        template <typename T>
        struct ContainerSerializable<std::vector<T>> {
        public:
            static constexpr bool val = DefaultSerializable<T> || CustomSerializable<T> || ContainerSerializable<T>::val;
        };
    };

    template <typename T>
    concept Serializable = DefaultSerializable<T> || CustomSerializable<T> || detail::ContainerSerializable<T>::val;

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

        template <Serializable T>
        std::vector<T> SerializerFactory () {
            return std::vector<T>{};
        }
    }

    class JsonSerializer {
    private:
        nlohmann::json json;
    public:
        JsonSerializer () : json{} {}

        bool asObject () {
            json = nlohmann::json::object();
            return true;
        }

        template <CustomSerializable T>
        bool visit (const T& val) {
            return CustomSerializer<T>::Accept(*this, val);
        }

        template <DefaultSerializable T>
        bool visit (const T& val) {
            json = val;
            return true;
        }

        template <Serializable T>
        bool visit (const std::vector<T>& val) {
            auto arr = nlohmann::json::array_t{};
            for (auto& i : val) {
                JsonSerializer serializer{};
                if (!serializer.visit(serializer, i)) {
                    return false;
                }

                arr.push_back(std::move(serializer.get()));
            }

            json = std::move(arr);

            return true;
        }

        template <std::size_t N, Serializable T>
        bool visitArray (const T* vals) {
            auto arr = nlohmann::json::array_t{};
            for (std::size_t i = 0; i < N; i++) {
                JsonSerializer serializer{};
                if (!serializer.visit(vals[i])) {
                    return false;
                }

                arr.push_back(std::move(serializer.get()));
            }

            json = std::move(arr);

            return true;
        }

        template <Serializable T>
        bool visitMember (T& member, const std::string& memberName) {
            if (!json.is_object()) {
                return false;
            }

            JsonSerializer serializer;
            if (!serializer.visit(member)) {
                return false;
            }

            json[memberName] = std::move(serializer.get());

            return true;
        }

        nlohmann::json& get () {
            return json;
        }

        template <Serializable T>
        static nlohmann::json Serialize (const T& val) {
            JsonSerializer serializer{};
            if (!serializer.visit(val)) {
                return nlohmann::json{};
            }

            return std::move(serializer.get());
        }
    };

    class JsonDeserializer {
    private:
        const nlohmann::json& json;
    public:
        JsonDeserializer (const nlohmann::json& json) : json{json} {}

        [[nodiscard]] bool asObject () const {
            if (json.is_object()) {
                return true;
            } else {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected object, got \"{}\"", json.type_name());
                return false;
            }
        }

        template <CustomSerializable T>
        bool visit (T& val) const {
            if (CustomSerializer<T>::Accept(*this, val)) {
                return true;
            } else {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Failed to parse {}", CustomSerializer<T>::Name);
                return false;
            }
        }

        template <Serializable T>
        bool visit (std::vector<T>& val) const {
            if (!json.is_array()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected list, got \"{}\"", json.type_name());
                return false;
            }

            const auto& arr = json.get<nlohmann::json::array_t>();
            std::vector<T> vec{};
            for (const auto& i : arr) {
                vec.emplace_back(detail::SerializerFactory<T>());
                if (!JsonDeserializer{i}.visit(vec.back())) {
                    return false;
                }
            }

            val = std::move(vec);

            return true;
        }

        template <std::size_t N, Serializable T>
        bool visitArray (T* vals) const {
            if (!json.is_array()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected array, got \"{}\"", json.type_name());
                return false;
            }

            const auto& arr = json.get<nlohmann::json::array_t>();
            if (arr.size() != N) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Array size is incorrect, expected {} got {}", N, arr.size());
                return false;
            }

            std::array<T, N> newVals{};
            for (std::size_t i = 0; i < N; i++) {
                if (!JsonDeserializer{arr.at(i)}.visit(newVals[i])) {
                    return false;
                }
            }

            for (std::size_t i = 0; i < N; i++) {
                vals[i] = std::move(newVals[i]);
            }

            return true;
        }

        template <Serializable T>
        bool visitMember (T& member, const std::string& memberName) const {
            if (!json.is_object()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected object, got \"{}\"", json.type_name());
                return false;
            }

            const auto& obj = json.get<nlohmann::json::object_t>();
            if (!obj.contains(memberName)) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Failed to find member \"{}\"", memberName);
                return false;
            }

            if (JsonDeserializer{obj.at(memberName)}.visit(member)) {
                return true;
            } else {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Failed to parse member \"{}\"", memberName);
                return false;
            }
        }

        [[nodiscard]] const nlohmann::json& get () const {
            return json;
        }

        bool visit (bool& val) const {
            if (!json.is_boolean()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected bool, got \"{}\"", json.type_name());
                return false;
            }

            val = json.get<bool>();
            return true;
        }

        bool visit (std::string& val) const {
            if (!json.is_string()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected string, got \"{}\"", json.type_name());
                return false;
            }

            val = json.get<std::string>();
            return true;
        }

        template <detail::SerializableFloat T>
        bool visit (T& val) const {
            if (!json.is_number()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected float, got \"{}\"", json.type_name());
                return false;
            }

            val = json.get<T>();
            return true;
        }

        template <detail::SerializableInt T>
        bool visit (T& val) const {
            if (!json.is_number_integer()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected int, got \"{}\"", json.type_name());
                return false;
            }

            val = json.get<T>();
            return true;
        }

        template <detail::SerializableUInt T>
        bool visit (T& val) const {
            if (!json.is_number_unsigned()) {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Expected uint, got \"{}\"", json.type_name());
                return false;
            }

            val = json.get<T>();
            return true;
        }

        template <typename T>
        util::Optional<T> deserialize () const {
            T val{detail::SerializerFactory<T>()};

            if (visit(val)) {
                return util::Optional<T>{std::move(val)};
            } else {
                PHENYL_LOGE(detail::SERIALIZER_LOGGER, "Failed to deserialize {}!", CustomSerializer<T>::Name);
                return util::NullOpt;
            }
        }

        template <typename T>
        static util::Optional<T> Deserialize (const nlohmann::json& json) {
            return JsonDeserializer{json}.deserialize<T>();
        }
    };
}

#define PHENYL_SERIALIZE_NAMED_INT(T, name, details, preVisit, postVisit)                         \
    struct phenyl_##T##_SerializerType {                                 \
        static constexpr const char* Name = name;                        \
                                                                         \
        static T Factory () {                                    \
            return T{};                                                  \
        }                                                                                         \
        \
        static bool Accept (::phenyl::common::JsonSerializer& visitor, const T& val) {   \
            constexpr bool IN_SERIALIZE = std::is_same_v<std::void_t<T>, void>;                          \
            constexpr bool IN_DESERIALIZE = !std::is_same_v<std::void_t<T>, void>;                       \
            using Type = T;                                              \
                                                                         \
            preVisit                                                                          \
                                        \
            details                                                                          \
            \
            postVisit\
            return true;                                                 \
        }                                                                                         \
        \
        static bool Accept (const ::phenyl::common::JsonDeserializer& visitor, T& val) { \
            constexpr bool IN_SERIALIZE = !std::is_same_v<std::void_t<T>, void>;                         \
            constexpr bool IN_DESERIALIZE = std::is_same_v<std::void_t<T>, void>;                        \
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
            if (!visitor.asObject()) {                                                            \
                return false;                                                                                     \
            }                                                                                       \
        }                                                                                               \
    }, {})

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
        if constexpr (IN_SERIALIZE) {                     \
            auto v = val.getMethod();\
            if (!visitor.visitMember(v, name)) { \
                return false;                                              \
            }   \
        }                                                 \
                                                          \
        if constexpr (IN_DESERIALIZE) {                \
            using ValType = std::remove_cvref_t<decltype(val.getMethod())>; \
            ValType newVal = ::phenyl::common::detail::SerializerFactory<ValType>();   \
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

