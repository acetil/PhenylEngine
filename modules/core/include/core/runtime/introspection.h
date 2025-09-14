#pragma once
#include "core/serialization/serializer.h"

#include <memory>
#include <string_view>
#include <unordered_map>

namespace phenyl::core {
enum class SerializedType {
    Primitive,
    Array,
    Map,
    Struct,
    Asset
};

class PrimitiveSerializedTypeInfo;
class ArraySerializedTypeInfo;
class MapSerializedTypeInfo;
class AssetSerializedTypeInfo;
class StructSerializedTypeInfo;

class ISerializedTypeInfo {
public:
    virtual ~ISerializedTypeInfo () = default;

    [[nodiscard]] virtual SerializedType type () const noexcept = 0;

    [[nodiscard]] const PrimitiveSerializedTypeInfo& asPrimitive () const noexcept;
    [[nodiscard]] const ArraySerializedTypeInfo& asArray () const noexcept;
    [[nodiscard]] const MapSerializedTypeInfo& asMap () const noexcept;
    [[nodiscard]] const AssetSerializedTypeInfo& asAsset () const noexcept;
    [[nodiscard]] const StructSerializedTypeInfo& asStruct () const noexcept;
};

class ComponentInfo {
public:
    ComponentInfo (std::string name, std::unique_ptr<ISerializedTypeInfo> typeInfo);

    [[nodiscard]] std::string_view name () const noexcept;
    [[nodiscard]] const ISerializedTypeInfo& typeInfo () const noexcept;

    static ComponentInfo GetInfo (std::string name, ISerializableBase& serializable);

    template <SerializableType T>
    static ComponentInfo GetInfo (std::string name) {
        auto& serializable = detail::GetSerializable<T>();
        return GetInfo(std::move(name), serializable);
    }

private:
    std::string m_name;
    std::unique_ptr<ISerializedTypeInfo> m_typeInfo;
};

enum class PrimitiveType {
    Bool,

    Int8,
    Int16,
    Int32,
    Int64,

    Uint8,
    Uint16,
    Uint32,
    Uint64,

    Float,
    Double,

    String,

    Vec2,
    Vec3,
    Vec4,

    Mat2,
    Mat3,
    Mat4,

    Quaternion
};

class PrimitiveSerializedTypeInfo : public ISerializedTypeInfo {
public:
    explicit PrimitiveSerializedTypeInfo (PrimitiveType primType);

    [[nodiscard]] SerializedType type () const noexcept override;
    [[nodiscard]] PrimitiveType primitiveType () const noexcept;

private:
    PrimitiveType m_primitiveType;
};

class ArraySerializedTypeInfo : public ISerializedTypeInfo {
public:
    explicit ArraySerializedTypeInfo (std::unique_ptr<ISerializedTypeInfo> elementType);

    [[nodiscard]] SerializedType type () const noexcept override;
    [[nodiscard]] const ISerializedTypeInfo& elementType () const noexcept;

private:
    std::unique_ptr<ISerializedTypeInfo> m_elementType;
};

class MapSerializedTypeInfo : public ISerializedTypeInfo {
public:
    explicit MapSerializedTypeInfo (std::unique_ptr<ISerializedTypeInfo> valueType);

    [[nodiscard]] SerializedType type () const noexcept override;
    [[nodiscard]] const ISerializedTypeInfo& valueType () const noexcept;

private:
    std::unique_ptr<ISerializedTypeInfo> m_valueType;
};

class AssetSerializedTypeInfo : public ISerializedTypeInfo {
public:
    explicit AssetSerializedTypeInfo (std::string asset);

    [[nodiscard]] SerializedType type () const noexcept override;
    [[nodiscard]] std::string_view asset () const noexcept;

private:
    std::string m_asset;
};

class StructSerializedTypeInfo : public ISerializedTypeInfo {
public:
    explicit StructSerializedTypeInfo (std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>> members);

    [[nodiscard]] SerializedType type () const noexcept override;
    [[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>>&
    members () const noexcept;

private:
    std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>> m_members;
};
} // namespace phenyl::core
