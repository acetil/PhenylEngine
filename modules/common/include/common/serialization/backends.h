#pragma once

#include <utility>

#include "serializer.h"

namespace phenyl::common {
    class JsonException : public DeserializeException {
    public:
        explicit JsonException (std::string message) : DeserializeException{std::move(message)} {}
    };

    void SerializeToJson (std::ostream& stream, ISerializableBase& serializable, const std::byte* ptr, bool pretty);
    std::string SerializeToJson (ISerializableBase& serializable, const std::byte* ptr, bool pretty);

    void DeserializeFromJson (std::istream& stream, ISerializableBase& serializable, std::byte* ptr);
    void DeserializeFromJson (std::string_view str, ISerializableBase& serializable, std::byte* ptr);

    template <typename T>
    void SerializeToJson (std::ostream& stream, ISerializable<T>& serializable, const T& obj, bool pretty = false) {
        SerializeToJson(stream, serializable, reinterpret_cast<const std::byte*>(&obj), pretty);
    }

    template <typename T>
    std::string SerializeToJson (ISerializable<T>& serializable, const T& obj, bool pretty = false) {
        return SerializeToJson(serializable, reinterpret_cast<const std::byte*>(&obj), pretty);
    }

    template <SerializableType T>
    void SerializeToJson (std::ostream& stream, const T& obj, bool pretty = false) {
        SerializeToJson(stream, detail::GetSerializable<T>(), reinterpret_cast<const std::byte*>(&obj), pretty);
    }

    template <SerializableType T>
    std::string SerializeToJson (const T& obj, bool pretty = false) {
        return SerializeToJson(detail::GetSerializable<T>(), reinterpret_cast<const std::byte*>(&obj), pretty);
    }

    template <SerializableType T>
    void DeserializeFromJson (std::istream& stream, ISerializable<T>& serializable, T& obj) {
        DeserializeFromJson(stream, serializable, reinterpret_cast<std::byte*>(&obj));
    }

    template <SerializableType T>
    void DeserializeFromJson (std::istream& stream, T& obj) {
        DeserializeFromJson(stream, detail::GetSerializable<T>(), obj);
    }

    template <SerializableType T>
    T DeserializeFromJson (std::istream& stream) {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        T obj = serializable.make();
        DeserializeFromJson(stream, serializable, reinterpret_cast<std::byte*>(&obj));

        return obj;
    }

    template <SerializableType T>
    void DeserializeFromJson (std::string_view str, ISerializable<T>& serializable, T& obj) {
        DeserializeFromJson(str, serializable, reinterpret_cast<std::byte*>(&obj));
    }

    template <SerializableType T>
    void DeserializeFromJson (std::string_view str, T& obj) {
        DeserializeFromJson(str, detail::GetSerializable<T>(), obj);
    }

    template <SerializableType T>
    T DeserializeFromJson (std::string_view str) {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        T obj = serializable.make();
        DeserializeFromJson(str, serializable, reinterpret_cast<std::byte*>(&obj));

        return obj;
    }
}