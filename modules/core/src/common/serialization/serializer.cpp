#include "core/serialization/serializer.h"

#include "core/serialization/serializer_impl.h"

using namespace phenyl::core;

namespace {
class BoolSerializable : public ISerializable<bool> {
    std::string_view name () const noexcept override {
        return "bool";
    }

    void serialize (ISerializer& serializer, const bool& obj) override {
        serializer.serialize(obj);
    }

    void deserialize (IDeserializer& deserializer, bool& obj) override {
        deserializer.deserializeBool(*this, obj);
    }

    void deserializeBool (bool& obj, bool val) override {
        obj = val;
    }
};

template <std::integral T>
class IntSerializable : public ISerializable<T> {
private:
    template <std::integral U>
    void deserializeOther (T& obj, U val) {
        if (!std::in_range<T>(val)) {
            throw phenyl::DeserializeException(std::format("Integer {} out of range", val));
        }

        obj = static_cast<T>(val);
    }

public:
    void serialize (ISerializer& serializer, const T& obj) final {
        serializer.serialize(obj);
    }

    void deserializeInt8 (T& obj, std::int8_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeInt16 (T& obj, std::int16_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeInt32 (T& obj, std::int32_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeInt64 (T& obj, std::int64_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeUint8 (T& obj, std::uint8_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeUint16 (T& obj, std::uint16_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeUint32 (T& obj, std::uint32_t val) override {
        deserializeOther(obj, val);
    }

    void deserializeUint64 (T& obj, std::uint64_t val) override {
        deserializeOther(obj, val);
    }
};

class Int8Serializable : public IntSerializable<std::int8_t> {
    std::string_view name () const noexcept override {
        return "std::int8_t";
    }

    void deserialize (IDeserializer& deserializer, std::int8_t& obj) override {
        deserializer.deserializeInt8(*this, obj);
    }
};

class Uint8Serializable : public IntSerializable<std::uint8_t> {
    std::string_view name () const noexcept override {
        return "std::uint8_t";
    }

    void deserialize (IDeserializer& deserializer, std::uint8_t& obj) override {
        deserializer.deserializeUint8(*this, obj);
    }
};

class Int16Serializable : public IntSerializable<std::int16_t> {
    std::string_view name () const noexcept override {
        return "std::int16_t";
    }

    void deserialize (IDeserializer& deserializer, std::int16_t& obj) override {
        deserializer.deserializeInt16(*this, obj);
    }
};

class Uint16Serializable : public IntSerializable<std::uint16_t> {
    std::string_view name () const noexcept override {
        return "std::uint16_t";
    }

    void deserialize (IDeserializer& deserializer, std::uint16_t& obj) override {
        deserializer.deserializeUint16(*this, obj);
    }
};

class Int32Serializable : public IntSerializable<std::int32_t> {
    std::string_view name () const noexcept override {
        return "std::int32_t";
    }

    void deserialize (IDeserializer& deserializer, std::int32_t& obj) override {
        deserializer.deserializeInt32(*this, obj);
    }
};

class Uint32Serializable : public IntSerializable<std::uint32_t> {
    std::string_view name () const noexcept override {
        return "std::uint32_t";
    }

    void deserialize (IDeserializer& deserializer, std::uint32_t& obj) override {
        deserializer.deserializeUint32(*this, obj);
    }
};

class Int64Serializable : public IntSerializable<std::int64_t> {
    std::string_view name () const noexcept override {
        return "std::int64_t";
    }

    void deserialize (IDeserializer& deserializer, std::int64_t& obj) override {
        deserializer.deserializeInt64(*this, obj);
    }
};

class Uint64Serializable : public IntSerializable<std::uint64_t> {
    std::string_view name () const noexcept override {
        return "std::uint64_t";
    }

    void deserialize (IDeserializer& deserializer, std::uint64_t& obj) override {
        deserializer.deserializeUint64(*this, obj);
    }
};

class FloatSerializable : public ISerializable<float> {
    std::string_view name () const noexcept override {
        return "float";
    }

    void serialize (ISerializer& serializer, const float& obj) override {
        serializer.serialize(obj);
    }

    void deserialize (IDeserializer& deserializer, float& obj) override {
        deserializer.deserializeFloat(*this, obj);
    }

    void deserializeFloat (float& obj, float val) override {
        obj = val;
    }

    void deserializeDouble (float& obj, double val) override {
        obj = static_cast<float>(val);
    }
};

class DoubleSerializable : public ISerializable<double> {
    std::string_view name () const noexcept override {
        return "double";
    }

    void serialize (ISerializer& serializer, const double& obj) override {
        serializer.serialize(obj);
    }

    void deserialize (IDeserializer& deserializer, double& obj) override {
        deserializer.deserializeDouble(*this, obj);
    }

    void deserializeDouble (double& obj, double val) override {
        obj = val;
    }
};

class StringSerializable : public ISerializable<std::string> {
    std::string_view name () const noexcept override {
        return "std::string";
    }

    void serialize (ISerializer& serializer, const std::string& obj) override {
        serializer.serialize(obj);
    }

    void deserialize (IDeserializer& deserializer, std::string& obj) override {
        deserializer.deserializeString(*this, obj);
    }

    void deserializeString (std::string& obj, std::string_view val) override {
        obj = val;
    }
};
} // namespace

ISerializable<bool>& detail::phenyl_GetSerializable (SerializableMarker<bool>) {
    static BoolSerializable serializable{};
    return serializable;
}

ISerializable<std::int8_t>& detail::phenyl_GetSerializable (SerializableMarker<std::int8_t>) {
    static Int8Serializable serializable{};
    return serializable;
}

ISerializable<std::uint8_t>& detail::phenyl_GetSerializable (SerializableMarker<std::uint8_t>) {
    static Uint8Serializable serializable{};
    return serializable;
}

ISerializable<std::int16_t>& detail::phenyl_GetSerializable (SerializableMarker<std::int16_t>) {
    static Int16Serializable serializable{};
    return serializable;
}

ISerializable<std::uint16_t>& detail::phenyl_GetSerializable (SerializableMarker<std::uint16_t>) {
    static Uint16Serializable serializable{};
    return serializable;
}

ISerializable<std::int32_t>& detail::phenyl_GetSerializable (SerializableMarker<std::int32_t>) {
    static Int32Serializable serializable{};
    return serializable;
}

ISerializable<std::uint32_t>& detail::phenyl_GetSerializable (SerializableMarker<std::uint32_t>) {
    static Uint32Serializable serializable{};
    return serializable;
}

ISerializable<std::int64_t>& detail::phenyl_GetSerializable (SerializableMarker<std::int64_t>) {
    static Int64Serializable serializable{};
    return serializable;
}

ISerializable<std::uint64_t>& detail::phenyl_GetSerializable (SerializableMarker<std::uint64_t>) {
    static Uint64Serializable serializable{};
    return serializable;
}

ISerializable<float>& detail::phenyl_GetSerializable (SerializableMarker<float>) {
    static FloatSerializable serializable{};
    return serializable;
}

ISerializable<double>& detail::phenyl_GetSerializable (SerializableMarker<double>) {
    static DoubleSerializable serializable{};
    return serializable;
}

ISerializable<std::string>& detail::phenyl_GetSerializable (SerializableMarker<std::string>) {
    static StringSerializable serializable{};
    return serializable;
}
