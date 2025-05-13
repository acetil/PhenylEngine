#include "core/serialization/backends.h"

#include <nlohmann/json.hpp>

using namespace phenyl::core;

class JsonSerializer : public ISerializer {
public:
    void serialize (bool val) override {
        m_json = val;
    }

    void serialize (std::int64_t val) override {
        m_json = val;
    }

    void serialize (std::uint64_t val) override {
        m_json = val;
    }

    void serialize (double val) override {
        m_json = val;
    }

    void serialize (std::string_view val) override {
        m_json = val;
    }

    IObjectSerializer& serializeObj () override {
        PHENYL_DASSERT(!m_arrSerializer);
        PHENYL_DASSERT(!m_objSerializer);

        m_objSerializer = Object{this};
        return *m_objSerializer;
    }

    IArraySerializer& serializeArr () override {
        PHENYL_DASSERT(!m_arrSerializer);
        PHENYL_DASSERT(!m_objSerializer);

        m_arrSerializer = Array{this};
        return *m_arrSerializer;
    }

    void onArrEnd (nlohmann::json::array_t&& arr) {
        PHENYL_DASSERT(m_arrSerializer);
        m_json = std::move(arr);
        m_arrSerializer = std::nullopt;
    }

    void onObjEnd (nlohmann::json::object_t&& obj) {
        m_json = std::move(obj);
        m_objSerializer = std::nullopt;
    }

    nlohmann::json& getJson () {
        return m_json;
    }

private:
    class Array : public IArraySerializer {
    public:
        Array (JsonSerializer* serializer) : m_serializer{serializer} {
            PHENYL_DASSERT(serializer);
        }

        void serializeElement (ISerializableBase& serializable, const std::byte* ptr) override {
            JsonSerializer serializer;
            serializable.serialize(serializer, ptr);

            m_arr.emplace_back(std::move(serializer.getJson()));
        }

        void end () override {
            m_serializer->onArrEnd(std::move(m_arr));
        }

    private:
        JsonSerializer* m_serializer;
        nlohmann::json::array_t m_arr{};
    };

    class Object : public IObjectSerializer {
    public:
        Object (JsonSerializer* serializer) : m_serializer{serializer} {
            PHENYL_DASSERT(serializer);
        }

        void serializeMember (std::string_view memberName, ISerializableBase& serializable,
            const std::byte* ptr) override {
            JsonSerializer serializer;
            serializable.serialize(serializer, ptr);

            m_obj.emplace(memberName, std::move(serializer.getJson()));
        }

        void end () override {
            m_serializer->onObjEnd(std::move(m_obj));
        }

        nlohmann::json::object_t& obj () {
            return m_obj;
        }

    private:
        JsonSerializer* m_serializer;
        nlohmann::json::object_t m_obj{};
    };

    nlohmann::json m_json;
    std::optional<Object> m_objSerializer;
    std::optional<Array> m_arrSerializer;
};

class JsonDeserializer : public IDeserializer {
public:
    explicit JsonDeserializer (const nlohmann::json& json) : m_json{json} {}

    void deserializeBool (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_boolean()) {
            serializable.deserializeBool(ptr, m_json.get<bool>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected bool, got {}", m_json.type_name()));
        }
    }

    void deserializeInt8 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeInt8(ptr, deserializeInt<std::int8_t>("std::int8_t"));
    }

    void deserializeInt16 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeInt16(ptr, deserializeInt<std::int16_t>("std::int16_t"));
    }

    void deserializeInt32 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeInt32(ptr, deserializeInt<std::int32_t>("std::int32_t"));
    }

    void deserializeInt64 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeInt64(ptr, deserializeInt<std::int64_t>("std::int64_t"));
    }

    void deserializeUint8 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeUint8(ptr, deserializeUint<std::uint8_t>("std::uint8_t"));
    }

    void deserializeUint16 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeUint16(ptr, deserializeUint<std::uint16_t>("std::uint16_t"));
    }

    void deserializeUint32 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeUint32(ptr, deserializeUint<std::uint32_t>("std::uint32_t"));
    }

    void deserializeUint64 (ISerializableBase& serializable, std::byte* ptr) override {
        serializable.deserializeUint64(ptr, deserializeUint<std::uint64_t>("std::uint64_t"));
    }

    void deserializeFloat (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_number()) {
            serializable.deserializeFloat(ptr, m_json.get<float>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected float, got {}", m_json.type_name()));
        }
    }

    void deserializeDouble (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_number()) {
            serializable.deserializeDouble(ptr, m_json.get<double>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected double, got {}", m_json.type_name()));
        }
    }

    void deserializeString (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_string()) {
            serializable.deserializeString(ptr, m_json.get<std::string>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected string, got {}", m_json.type_name()));
        }
    }

    void deserializeArray (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_array()) {
            Array arr{*m_json.get<const nlohmann::json::array_t*>()};
            serializable.deserializeArray(ptr, arr);
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected array, got {}", m_json.type_name()));
        }
    }

    void deserializeObject (ISerializableBase& serializable, std::byte* ptr) override {
        if (m_json.is_object()) {
            Object obj{*m_json.get<const nlohmann::json::object_t*>()};
            serializable.deserializeObject(ptr, obj);
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected object, got {}", m_json.type_name()));
        }
    }

    void deserializeStruct (ISerializableBase& serializable, std::span<const std::string> members,
        std::byte* ptr) override {
        if (m_json.is_object()) {
            Struct obj{*m_json.get<const nlohmann::json::object_t*>(), members};
            serializable.deserializeStruct(ptr, obj);
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected object, got {}", m_json.type_name()));
        }
    }

    void deserializeInfer (ISerializableBase& serializable, std::byte* ptr) override {
        switch (m_json.type()) {
        case nlohmann::detail::value_t::null:
            throw JsonException("Failed to deserialize json: received null value");
            break;
        case nlohmann::detail::value_t::object:
            deserializeObject(serializable, ptr);
            break;
        case nlohmann::detail::value_t::array:
            deserializeArray(serializable, ptr);
            break;
        case nlohmann::detail::value_t::string:
            deserializeString(serializable, ptr);
            break;
        case nlohmann::detail::value_t::boolean:
            deserializeBool(serializable, ptr);
            break;
        case nlohmann::detail::value_t::number_integer:
            deserializeInt64(serializable, ptr);
            break;
        case nlohmann::detail::value_t::number_unsigned:
            deserializeUint64(serializable, ptr);
            break;
        case nlohmann::detail::value_t::number_float:
            deserializeObject(serializable, ptr);
            break;
        case nlohmann::detail::value_t::binary:
            throw JsonException("Failed to deserialize json: binary arrays are not supported");
            break;
        case nlohmann::detail::value_t::discarded:
            throw JsonException("Failed to deserialize json: received error in parsing!");
            break;
        }
    }

private:
    class Array : public IArrayDeserializer {
    public:
        using ArrType = nlohmann::json::array_t;

        Array (const ArrType& arr) : m_arr{arr}, m_it{arr.begin()} {}

        bool hasNext () const override {
            return m_it != m_arr.end();
        }

        void next (ISerializableBase& serializable, std::byte* obj) override {
            PHENYL_DASSERT(m_it != m_arr.end());
            JsonDeserializer deserializer{*m_it};
            serializable.deserialize(deserializer, obj);
            ++m_it;
        }

    private:
        const ArrType& m_arr;
        ArrType::const_iterator m_it;
    };

    class Object : public IObjectDeserializer {
    public:
        using ObjectType = nlohmann::json::object_t;

        explicit Object (const ObjectType& obj) : m_obj{obj}, m_it{obj.begin()} {}

        [[nodiscard]] bool hasNext () const override {
            return m_it != m_obj.end();
        }

        std::string_view nextKey () override {
            PHENYL_DASSERT(m_it != m_obj.end());
            return m_it->first;
        }

        void nextValue (ISerializableBase& serializable, std::byte* obj) override {
            PHENYL_DASSERT(m_it != m_obj.end());
            JsonDeserializer deserializer{m_it->second};
            serializable.deserialize(deserializer, obj);

            ++m_it;
        }

        void ignoreNextValue () override {
            PHENYL_DASSERT(m_it != m_obj.end());
            ++m_it;
        }

    private:
        const ObjectType& m_obj;
        ObjectType::const_iterator m_it;
    };

    class Struct : public IStructDeserializer {
    public:
        using ObjectType = nlohmann::json::object_t;

        Struct (const ObjectType& obj, std::span<const std::string> members) :
            m_obj{obj},
            m_members{members},
            m_memberIt{members.begin()} {
            advanceToNext();
        }

        bool isNext (std::string_view member) override {
            return m_memberIt != m_members.end() && member == *m_memberIt;
        }

        void next (ISerializableBase& serializable, std::byte* obj) override {
            PHENYL_DASSERT(m_memberIt != m_members.end());
            const auto& jsonMember = m_obj.at(*m_memberIt);

            JsonDeserializer deserializer{jsonMember};
            serializable.deserialize(deserializer, obj);

            ++m_memberIt;
            advanceToNext();
        }

    private:
        const ObjectType& m_obj;
        std::span<const std::string> m_members;
        std::span<const std::string>::iterator m_memberIt;

        void advanceToNext () {
            while (m_memberIt != m_members.end() && !m_obj.contains(*m_memberIt)) {
                ++m_memberIt;
            }
        }
    };

    const nlohmann::json& m_json;

    template<std::integral T>
    T deserializeInt (std::string_view typeName) {
        if (m_json.is_number_integer()) {
            auto i = m_json.get<nlohmann::json::number_integer_t>();

            if (std::in_range<T>(i)) {
                return static_cast<T>(i);
            } else {
                throw JsonException(
                    std::format("Failed to deserialize integer: {} out of bounds of type {}", i, typeName));
            }
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected int, got {}", m_json.type_name()));
        }
    }

    template<std::unsigned_integral T>
    T deserializeUint (std::string_view typeName) {
        if (m_json.is_number_unsigned()) {
            auto i = m_json.get<nlohmann::json::number_unsigned_t>();

            if (std::in_range<T>(i)) {
                return static_cast<T>(i);
            } else {
                throw JsonException(
                    std::format("Failed to deserialize unsigned integer: {} out of bounds of type {}", i, typeName));
            }
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected uint, got {}", m_json.type_name()));
        }
    }
};

void phenyl::core::SerializeToJson (std::ostream& stream, ISerializableBase& serializable, const std::byte* ptr,
    bool pretty) {
    JsonSerializer serializer{};
    serializable.serialize(serializer, ptr);

    stream << serializer.getJson().dump(pretty ? 4 : -1);
}

std::string phenyl::core::SerializeToJson (ISerializableBase& serializable, const std::byte* ptr, bool pretty) {
    JsonSerializer serializer;
    serializable.serialize(serializer, ptr);

    return serializer.getJson().dump(pretty ? 4 : -1);
}

void phenyl::core::DeserializeFromJson (std::istream& stream, ISerializableBase& serializable, std::byte* ptr) {
    try {
        auto json = nlohmann::json::parse(stream);
        JsonDeserializer deserializer{json};
        serializable.deserialize(deserializer, ptr);
    } catch (const nlohmann::json::exception& e) {
        throw DeserializeException(std::format("Failed to deserialize from json: {}", e.what()));
    }
}

void phenyl::core::DeserializeFromJson (std::string_view str, ISerializableBase& serializable, std::byte* ptr) {
    try {
        auto json = nlohmann::json::parse(str);
        JsonDeserializer deserializer{json};
        serializable.deserialize(deserializer, ptr);
    } catch (const nlohmann::json::exception& e) {
        throw DeserializeException(std::format("Failed to deserialize from json: {}", e.what()));
    }
}
