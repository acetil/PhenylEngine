#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "logging/logging.h"
#include "util/exceptions.h"

#include "serializer_forward.h"

namespace phenyl {
    class DeserializeException : public PhenylException {
    public:
        explicit DeserializeException (std::string message) : PhenylException{std::move(message)} {}
    };
}

namespace phenyl::common {
    class ISerializableBase;
    template <typename T>
    class ISerializable;

    class ISerializer;
    class IObjectSerializer;
    class IArraySerializer;

    class IDeserializer;
    class IObjectDeserializer;
    class IArrayDeserializer;

    class ISerializableBase {
    public:
        virtual ~ISerializableBase () = default;

        [[nodiscard]] virtual std::string_view name () const noexcept = 0;

        virtual void serialize (ISerializer& serializer, const std::byte* ptr) = 0;

        virtual void deserialize (IDeserializer& deserializer, std::byte* ptr) = 0;

        virtual void deserializeBool (std::byte* ptr, bool val) = 0;

        virtual void deserializeInt8 (std::byte* ptr, std::int8_t val) = 0;
        virtual void deserializeInt16 (std::byte* ptr, std::int16_t val) = 0;
        virtual void deserializeInt32 (std::byte* ptr, std::int32_t val) = 0;
        virtual void deserializeInt64 (std::byte* ptr, std::int64_t val) = 0;

        virtual void deserializeUint8 (std::byte* ptr, std::uint8_t val) = 0;
        virtual void deserializeUint16 (std::byte* ptr, std::uint16_t val) = 0;
        virtual void deserializeUint32 (std::byte* ptr, std::uint32_t val) = 0;
        virtual void deserializeUint64 (std::byte* ptr, std::uint64_t val) = 0;

        virtual void deserializeFloat (std::byte* ptr, float val) = 0;
        virtual void deserializeDouble (std::byte* ptr, double val) = 0;

        virtual void deserializeString (std::byte* ptr, std::string_view val) = 0;

        virtual void deserializeArray (std::byte* ptr, IArrayDeserializer& deserializer) = 0;
        virtual void deserializeObject (std::byte* ptr, IObjectDeserializer& deserializer) = 0;
    };

    template <typename T>
    class ISerializable : private ISerializableBase {
    private:
        void serialize (ISerializer& serializer, const std::byte* ptr) final {
            PHENYL_DASSERT(ptr);
            serialize(serializer, *reinterpret_cast<const T*>(ptr));
        }

        void deserialize (IDeserializer& deserializer, std::byte* ptr) final {
            PHENYL_DASSERT(ptr);
            deserialize(deserializer, *reinterpret_cast<T*>(ptr));
        }

        void deserializeBool (std::byte* ptr, bool val) final {
            PHENYL_DASSERT(ptr);
            deserializeBool(*reinterpret_cast<T*>(ptr), val);
        }

        void deserializeInt8 (std::byte* ptr, std::int8_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeInt8(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeInt16 (std::byte* ptr, std::int16_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeInt16(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeInt32 (std::byte* ptr, std::int32_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeInt32(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeInt64 (std::byte* ptr, std::int64_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeInt64(*reinterpret_cast<T*>(ptr), val);
        }

        void deserializeUint8 (std::byte* ptr, std::uint8_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeUint8(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeUint16 (std::byte* ptr, std::uint16_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeUint16(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeUint32 (std::byte* ptr, std::uint32_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeUint32(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeUint64 (std::byte* ptr, std::uint64_t val) final {
            PHENYL_DASSERT(ptr);
            deserializeUint64(*reinterpret_cast<T*>(ptr), val);
        }

        void deserializeFloat (std::byte* ptr, float val) final {
            PHENYL_DASSERT(ptr);
            deserializeFloat(*reinterpret_cast<T*>(ptr), val);
        }
        void deserializeDouble (std::byte* ptr, double val) final {
            PHENYL_DASSERT(ptr);
            deserializeDouble(*reinterpret_cast<T*>(ptr), val);
        }

        void deserializeString (std::byte* ptr, std::string_view val) final {
            PHENYL_DASSERT(ptr);
            deserializeString(*reinterpret_cast<T*>(ptr), val);
        }

        void deserializeArray (std::byte* ptr, IArrayDeserializer& deserializer) final {
            PHENYL_DASSERT(ptr);
            deserializeArray(*reinterpret_cast<T*>(ptr), deserializer);
        }

        void deserializeObject(std::byte* ptr, IObjectDeserializer& deserializer) final {
            PHENYL_DASSERT(ptr);
            deserializeObject(*reinterpret_cast<T*>(ptr), deserializer);
        }

        friend class ISerializer;
        friend class IObjectSerializer;
        friend class IArraySerializer;

        friend class IDeserializer;
        friend class IObjectDeserializer;
        friend class IArrayDeserializer;
    public:
        virtual T make () const {
            return T{};
        }

        virtual void serialize (ISerializer& serializer, const T& obj) = 0;

        virtual void deserialize (IDeserializer& deserializer, T& obj) = 0;

        virtual void deserializeBool (T& obj, bool val) {
            throw DeserializeException("Attempted to deserialize bool to type that doesn't support it!");
        }

        virtual void deserializeInt8 (T& obj, std::int8_t val) {
            deserializeInt16(obj, val);
        }
        virtual void deserializeInt16 (T& obj, std::int16_t val) {
            deserializeInt32(obj, val);
        }
        virtual void deserializeInt32 (T& obj, std::int32_t val) {
            deserializeInt64(obj, val);
        }
        virtual void deserializeInt64 (T& obj, std::int64_t val) {
            throw DeserializeException("Attempted to deserialize int64 to type that doesn't support it!");
        }

        virtual void deserializeUint8 (T& obj, std::uint8_t val) {
            deserializeUint16(obj, val);
        }
        virtual void deserializeUint16 (T& obj, std::uint16_t val) {
            deserializeUint32(obj, val);
        }
        virtual void deserializeUint32 (T& obj, std::uint32_t val) {
            deserializeUint64(obj, val);
        }
        virtual void deserializeUint64 (T& obj, std::uint64_t val) {
            throw DeserializeException("Attempted to deserialize uint64 to type that doesn't support it!");
        }

        virtual void deserializeFloat (T& obj, float val) {
            deserializeDouble(obj, static_cast<double>(val));
        }
        virtual void deserializeDouble (T& obj, double val) {
            throw DeserializeException("Attempted to deserialize double to type that doesn't support it!");
        }

        virtual void deserializeString (T& obj, std::string_view val) {
            throw DeserializeException("Attempted to deserialize string to type that doesn't support it!");
        }

        virtual void deserializeArray (T& obj, IArrayDeserializer& deserializer) {
            throw DeserializeException("Attempted to deserialize array to type that doesn't support it!");
        }
        virtual void deserializeObject (T& obj, IObjectDeserializer& deserializer) {
            throw DeserializeException("Attempted to deserialize object to type that doesn't support it!");
        }
    };

    class ISerializer {
    public:
        virtual ~ISerializer () = default;

        virtual void serialize (bool val) = 0;

        virtual void serialize (std::int8_t val) {
            serialize(static_cast<std::int64_t>(val));
        }
        virtual void serialize (std::int16_t val) {
            serialize(static_cast<std::int64_t>(val));
        }
        virtual void serialize (std::int32_t val) {
            serialize(static_cast<std::int64_t>(val));
        }
        virtual void serialize (std::int64_t val) = 0;

        virtual void serialize (std::uint8_t val) {
            serialize(static_cast<std::uint64_t>(val));
        }
        virtual void serialize (std::uint16_t val) {
            serialize(static_cast<std::uint64_t>(val));
        }
        virtual void serialize (std::uint32_t val) {
            serialize(static_cast<std::uint64_t>(val));
        }
        virtual void serialize (std::uint64_t val) = 0;

        virtual void serialize (float val) {
            serialize(static_cast<double>(val));
        }
        virtual void serialize (double val) = 0;

        virtual void serialize (const std::string& val) {
            serialize(std::string_view{val});
        }
        virtual void serialize (std::string_view val) = 0;

        template <SerializableType T>
        void serialize (const T& obj) {
            ISerializable<T>& serializable = detail::GetSerializable<T>();
            serializable.serialize(*this, obj);
        }

        virtual IObjectSerializer& serializeObj () = 0;
        virtual IArraySerializer& serializeArr () = 0;
    };

    class IObjectSerializer {
    protected:
        virtual void serializeMember (std::string_view memberName, ISerializableBase& serializable, const std::byte* ptr) = 0;
    public:
        virtual ~IObjectSerializer () = default;

        template <SerializableType T>
        void serializeMember (std::string_view memberName, const T& obj) {
            serializeMember(memberName, detail::GetSerializable<T>(), reinterpret_cast<const std::byte*>(&obj));
        }
        virtual void end () = 0;
    };

    class IArraySerializer {
    protected:
        virtual void serializeElement (ISerializableBase& serializable, const std::byte* ptr) = 0;
    public:
        virtual ~IArraySerializer () = default;

        template <SerializableType T>
        void serializeElement (const T& obj) {
            serializeElement(detail::GetSerializable<T>(), reinterpret_cast<const std::byte*>(&obj));
        }
        virtual void end () = 0;
    };

    class IDeserializer {
    protected:
        virtual void deserializeBool (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeInt8 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeInt16 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeInt32 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeInt64 (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeUint8 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeUint16 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeUint32 (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeUint64 (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeFloat (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeDouble (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeString (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeObject (ISerializableBase& serializable, std::byte* ptr) = 0;
        virtual void deserializeArray (ISerializableBase& serializable, std::byte* ptr) = 0;

        virtual void deserializeInfer (ISerializableBase& serializable, std::byte* ptr) = 0;
    public:
        virtual ~IDeserializer () = default;

        template <SerializableType T>
        void deserializeBool (ISerializable<T>& serializable, T& obj) {
            deserializeBool(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeInt8 (ISerializable<T>& serializable, T& obj) {
            deserializeInt8(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeInt16 (ISerializable<T>& serializable, T& obj) {
            deserializeInt16(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeInt32 (ISerializable<T>& serializable, T& obj) {
            deserializeInt32(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeInt64 (ISerializable<T>& serializable, T& obj) {
            deserializeInt64(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeUint8 (ISerializable<T>& serializable, T& obj) {
            deserializeUint8(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeUint16 (ISerializable<T>& serializable, T& obj) {
            deserializeUint16(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeUint32 (ISerializable<T>& serializable, T& obj) {
            deserializeUint32(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeUint64 (ISerializable<T>& serializable, T& obj) {
            deserializeUint64(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeFloat (ISerializable<T>& serializable, T& obj) {
            deserializeFloat(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeDouble (ISerializable<T>& serializable, T& obj) {
            deserializeDouble(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeString (ISerializable<T>& serializable, T& obj) {
            deserializeString(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeObject (ISerializable<T>& serializable, T& obj) {
            deserializeObject(serializable, reinterpret_cast<std::byte*>(&obj));
        }
        template <SerializableType T>
        void deserializeArray (ISerializable<T>& serializable, T& obj) {
            deserializeArray(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        void deserializeInfer (ISerializable<T>& serializable, T& obj) {
            deserializeInfer(serializable, reinterpret_cast<std::byte*>(&obj));
        }

        template <SerializableType T>
        T deserialize () {
            ISerializable<T>& serializable = detail::GetSerializable<T>();
            T obj{serializable.make()};
            serializable.deserialize(*this, obj);

            return obj;
        }
    };

    class IArrayDeserializer {
    protected:
        virtual void next (ISerializableBase& serializable, std::byte* obj);
    public:
        virtual ~IArrayDeserializer () = default;

        [[nodiscard]] virtual std::optional<std::size_t> tryGetSize () const {
            return std::nullopt;
        }
        [[nodiscard]] virtual bool hasNext () const = 0;

        template <typename T>
        T next () {
            ISerializable<T>& serializable = detail::GetSerializable<T>();
            T obj{serializable.make()};
            next(serializable, reinterpret_cast<std::byte*>(&obj));

            return obj;
        }
    };

    class IObjectDeserializer {
    protected:
        virtual void nextValue (ISerializableBase& serializable, std::byte* obj);
    public:
        virtual ~IObjectDeserializer () = default;

        [[nodiscard]] virtual bool hasNext () const = 0;

        virtual std::string_view nextKey () = 0;
        template <typename T>
        T nextValue () {
            ISerializable<T>& serializable = detail::GetSerializable<T>();
            T obj{serializable.make()};
            nextValue(serializable, reinterpret_cast<std::byte*>(&obj));

            return obj;
        }

        template <typename T>
        void nextValue (T& obj) {
            nextValue(detail::GetSerializable<T>(), reinterpret_cast<std::byte*>(&obj));
        }
    };
}