#include <nlohmann/json.hpp>

#include "common/serialization/backends.h"

using namespace phenyl::common;

class JsonSerializer : public ISerializer {
private:
    class Array : public IArraySerializer {
    private:
        JsonSerializer* serializer;
        nlohmann::json::array_t jsonArr{};
    public:
        Array (JsonSerializer* serializer) : serializer{serializer} {
            PHENYL_DASSERT(serializer);
        }

        void serializeElement (ISerializableBase& serializable, const std::byte* ptr) override {
            JsonSerializer serializer;
            serializable.serialize(serializer, ptr);

            jsonArr.emplace_back(std::move(serializer.getJson()));
        }

        void end () override {
            serializer->onArrEnd(std::move(jsonArr));
        }
    };

    class Object : public IObjectSerializer {
    private:
        JsonSerializer* serializer;
        nlohmann::json::object_t jsonObj{};
    public:
        Object (JsonSerializer* serializer) : serializer{serializer} {
            PHENYL_DASSERT(serializer);
        }

        void serializeMember (std::string_view memberName, ISerializableBase& serializable, const std::byte* ptr) override {
            JsonSerializer serializer;
            serializable.serialize(serializer, ptr);

            jsonObj.emplace(memberName, std::move(serializer.getJson()));
        }

        void end () override {
            serializer->onObjEnd(std::move(jsonObj));
        }

        nlohmann::json::object_t& getObj () {
            return jsonObj;
        }
    };

    nlohmann::json json;
    std::optional<Object> objSerializer;
    std::optional<Array> arrSerializer;
public:
    void serialize (bool val) override {
        json = val;
    }

    void serialize (std::int64_t val) override {
        json = val;
    }

    void serialize (std::uint64_t val) override {
        json = val;
    }

    void serialize (double val) override {
        json = val;
    }

    void serialize (std::string_view val) override {
        json = val;
    }

    IObjectSerializer& serializeObj () override {
        PHENYL_DASSERT(!arrSerializer);
        PHENYL_DASSERT(!objSerializer);

        objSerializer = Object{this};
        return *objSerializer;
    }

    IArraySerializer& serializeArr() override {
        PHENYL_DASSERT(!arrSerializer);
        PHENYL_DASSERT(!objSerializer);

        arrSerializer = Array{this};
        return *arrSerializer;
    }

    void onArrEnd (nlohmann::json::array_t&& arr) {
        PHENYL_DASSERT(arrSerializer);
        json = std::move(arr);
        arrSerializer = std::nullopt;
    }

    void onObjEnd (nlohmann::json::object_t&& obj) {
        json = std::move(obj);
        objSerializer = std::nullopt;
    }

    nlohmann::json& getJson () {
        return json;
    }
};

class JsonDeserializer : public IDeserializer {
private:
    class Array : public IArrayDeserializer {
    private:
        using ArrType = nlohmann::json::array_t;

        const ArrType& jsonArr;
        ArrType::const_iterator it;
    public:
        Array (const ArrType& arr) : jsonArr{arr}, it{arr.begin()} {}

        bool hasNext () const override {
            return it != jsonArr.end();
        }

        void next (ISerializableBase& serializable, std::byte* obj) override {
            PHENYL_DASSERT(it != jsonArr.end());
            JsonDeserializer deserializer{*it};
            serializable.deserialize(deserializer, obj);
            ++it;
        }
    };

    class Object : public IObjectDeserializer {
    private:
        using ObjectType = nlohmann::json::object_t;

        const ObjectType& jsonObj;
        ObjectType::const_iterator it;
    public:
        explicit Object (const ObjectType& obj) : jsonObj{obj}, it{obj.begin()} {}

        [[nodiscard]] bool hasNext () const override {
            return it != jsonObj.end();
        }

        std::string_view nextKey () override {
            PHENYL_DASSERT(it != jsonObj.end());
            return it->first;
        }

        void nextValue (ISerializableBase& serializable, std::byte* obj) override {
            PHENYL_DASSERT(it != jsonObj.end());
            JsonDeserializer deserializer{it->second};
            serializable.deserialize(deserializer, obj);

            ++it;
        }

        void ignoreNextValue () override {
            PHENYL_DASSERT(it != jsonObj.end());
            ++it;
        }
    };

    const nlohmann::json& json;

    template <std::integral T>
    T deserializeInt (std::string_view typeName) {
        if (json.is_number_integer()) {
            auto i = json.get<nlohmann::json::number_integer_t>();

            if (std::in_range<T>(i)) {
                return static_cast<T>(i);
            } else {
                throw JsonException(std::format("Failed to deserialize integer: {} out of bounds of type {}", i, typeName));
            }
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected int, got {}", json.type_name()));
        }
    }

    template <std::unsigned_integral T>
    T deserializeUint (std::string_view typeName) {
        if (json.is_number_unsigned()) {
            auto i = json.get<nlohmann::json::number_unsigned_t>();

            if (std::in_range<T>(i)) {
                return static_cast<T>(i);
            } else {
                throw JsonException(std::format("Failed to deserialize unsigned integer: {} out of bounds of type {}", i, typeName));
            }
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected uint, got {}", json.type_name()));
        }
    }
public:
    explicit JsonDeserializer (const nlohmann::json& json) : json{json} {}

    void deserializeBool (ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_boolean()) {
            serializable.deserializeBool(ptr, json.get<bool>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected bool, got {}", json.type_name()));
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

    void deserializeFloat(ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_number()) {
            serializable.deserializeFloat(ptr, json.get<float>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected float, got {}", json.type_name()));
        }
    }

    void deserializeDouble (ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_number()) {
            serializable.deserializeDouble(ptr, json.get<double>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected double, got {}", json.type_name()));
        }
    }

    void deserializeString (ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_string()) {
            serializable.deserializeString(ptr, json.get<std::string>());
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected string, got {}", json.type_name()));
        }
    }

    void deserializeArray (ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_array()) {
            Array arr{*json.get<const nlohmann::json::array_t*>()};
            serializable.deserializeArray(ptr, arr);
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected array, got {}", json.type_name()));
        }
    }

    void deserializeObject (ISerializableBase& serializable, std::byte* ptr) override {
        if (json.is_object()) {
            Object obj{*json.get<const nlohmann::json::object_t*>()};
            serializable.deserializeObject(ptr, obj);
        } else {
            throw JsonException(std::format("Failed to deserialize json: expected object, got {}", json.type_name()));
        }
    }

    void deserializeInfer (ISerializableBase& serializable, std::byte* ptr) override {
        switch (json.type()) {
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
};

void phenyl::common::SerializeToJson (std::ostream& stream, ISerializableBase& serializable, const std::byte* ptr, bool pretty) {
    JsonSerializer serializer{};
    serializable.serialize(serializer, ptr);

    stream << serializer.getJson().dump(pretty ? 4 : -1);
}

std::string phenyl::common::SerializeToJson (ISerializableBase& serializable, const std::byte* ptr, bool pretty) {
    JsonSerializer serializer;
    serializable.serialize(serializer, ptr);

    return serializer.getJson().dump(pretty ? 4 : -1);
}

void phenyl::common::DeserializeFromJson (std::istream& stream, ISerializableBase& serializable, std::byte* ptr) {
    try {
        auto json = nlohmann::json::parse(stream);
        JsonDeserializer deserializer{json};
        serializable.deserialize(deserializer, ptr);
    } catch (const nlohmann::json::exception& e) {
        throw DeserializeException(std::format("Failed to deserialize from json: {}", e.what()));
    }
}

void phenyl::common::DeserializeFromJson (std::string_view str, ISerializableBase& serializable, std::byte* ptr) {
    try {
        auto json = nlohmann::json::parse(str);
        JsonDeserializer deserializer{json};
        serializable.deserialize(deserializer, ptr);
    } catch (const nlohmann::json::exception& e) {
        throw DeserializeException(std::format("Failed to deserialize from json: {}", e.what()));
    }
}