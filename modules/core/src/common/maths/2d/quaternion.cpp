#include "core/maths/3d/quaternion.h"
#include "core/serialization/serializer_impl.h"

using namespace phenyl::core;

class QuaternionSerializable : public ISerializable<Quaternion> {
public:
    std::string_view name () const noexcept override {
        return "Quaternion";
    }

    void serialize (ISerializer& serializer, const Quaternion& obj) override {
        auto& objSerializer = serializer.serializeObj();
        objSerializer.serializeMember("w", obj.w());
        objSerializer.serializeMember("xyz", obj.xyz());
    }

    void deserialize (IDeserializer& deserializer, Quaternion& obj) override {
        static std::string members[] = {"w", "xyz"};
        deserializer.deserializeStruct(*this, members, obj);
    }

    void deserializeStruct (Quaternion& obj, IStructDeserializer& deserializer) override {
        auto wOpt = deserializer.next<float>("w");
        if (!wOpt) {
            throw phenyl::DeserializeException("Failed to deserialize w from Quaternion");
        }

        auto xyzOpt = deserializer.next<glm::vec3>("xyz");
        if (!xyzOpt) {
            throw phenyl::DeserializeException("Failed to deserialize xyz from Quaternion");
        }

        obj = Quaternion{*wOpt, *xyzOpt};
    }
};

namespace phenyl::core {
    ISerializable<Quaternion>& phenyl_GetSerializable (detail::SerializableMarker<Quaternion>) {
        static QuaternionSerializable serializable{};
        return serializable;
    }
}