#include "core/runtime/introspection.h"

#include "core/detail/loggers.h"
#include "core/serialization/schema.h"

using namespace phenyl::core;

static phenyl::Logger LOGGER{"INTROSPECTION", detail::COMMON_LOGGER};

namespace {
class SerializedTypeVisitor : public ISchemaVisitor {
public:
    std::unique_ptr<ISerializedTypeInfo> info = nullptr;

    void visitBool () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Bool);
    }

    void visitInt8 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Int8);
    }

    void visitInt16 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Int16);
    }

    void visitInt32 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Int32);
    }

    void visitInt64 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Int64);
    }

    void visitUint8 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Uint8);
    }

    void visitUint16 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Uint16);
    }

    void visitUint32 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Uint32);
    }

    void visitUint64 () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Uint64);
    }

    void visitFloat () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Float);
    }

    void visitDouble () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Double);
    }

    void visitString () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::String);
    }

    void visitArray (ISerializableBase& serializable) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        SerializedTypeVisitor elemVisitor{};
        elemVisitor.visit(serializable);
        info = std::make_unique<ArraySerializedTypeInfo>(std::move(elemVisitor.info));
    }

    void visitMap (ISerializableBase& serializable) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        SerializedTypeVisitor valVisitor{};
        valVisitor.visit(serializable);
        info = std::make_unique<MapSerializedTypeInfo>(std::move(valVisitor.info));
    }

    void pushStruct (std::string_view name) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        m_structInfo = StructInfo{};
    }

    void visitMember (std::string_view memberName, ISerializableBase& serializable) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(m_structInfo);
        SerializedTypeVisitor memberVisitor{};
        memberVisitor.visit(serializable);
        m_structInfo->members.emplace(memberName, std::move(memberVisitor.info));
    }

    void popStruct () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(m_structInfo);
        info = std::make_unique<StructSerializedTypeInfo>(std::move(m_structInfo->members));
    }

    void visitEngineType (EngineSerializableType type) override {
        PHENYL_LOGI(LOGGER, "Skipping engine type");
    }

    void visitVector (ISerializableBase& type, glm::length_t N) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        if (N == 2) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Vec2);
        } else if (N == 3) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Vec3);
        } else if (N == 4) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Vec4);
        } else {
            PHENYL_LOGW(LOGGER, "Unsupported vector length: {}", N);
        }
    }

    void visitMatrix (ISerializableBase& type, glm::length_t H, glm::length_t W) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        if (H == 2 && W == 2) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Mat2);
        } else if (H == 3 && W == 3) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Mat3);
        } else if (H == 4 && W == 4) {
            info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Mat4);
        } else {
            PHENYL_LOGW(LOGGER, "Unsupported matrix: {}x{}", H, W);
        }
    }

    void visitQuaternion () override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<PrimitiveSerializedTypeInfo>(PrimitiveType::Quaternion);
    }

    void visitAsset (std::string_view assetName) override {
        PHENYL_DASSERT(!info);
        PHENYL_DASSERT(!m_structInfo);
        info = std::make_unique<AssetSerializedTypeInfo>(std::string{assetName});
    }

    void visit (ISerializableBase& serializable) override {
        serializable.accept(*this);
    }

private:
    struct StructInfo {
        std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>> members;
    };

    std::optional<StructInfo> m_structInfo;
};
} // namespace

const PrimitiveSerializedTypeInfo& ISerializedTypeInfo::asPrimitive () const noexcept {
    PHENYL_DASSERT_MSG(type() == SerializedType::Primitive, "Attempted to cast non-primitive type to Primitive");
    return reinterpret_cast<const PrimitiveSerializedTypeInfo&>(*this);
}

const ArraySerializedTypeInfo& ISerializedTypeInfo::asArray () const noexcept {
    PHENYL_DASSERT_MSG(type() == SerializedType::Array, "Attempted to cast non-ar type to Array");
    return reinterpret_cast<const ArraySerializedTypeInfo&>(*this);
}

const MapSerializedTypeInfo& ISerializedTypeInfo::asMap () const noexcept {
    PHENYL_DASSERT_MSG(type() == SerializedType::Map, "Attempted to cast non-map type to Map");
    return reinterpret_cast<const MapSerializedTypeInfo&>(*this);
}

const AssetSerializedTypeInfo& ISerializedTypeInfo::asAsset () const noexcept {
    PHENYL_DASSERT_MSG(type() == SerializedType::Asset, "Attempted to cast non-asset type to Asset");
    return reinterpret_cast<const AssetSerializedTypeInfo&>(*this);
}

const StructSerializedTypeInfo& ISerializedTypeInfo::asStruct () const noexcept {
    PHENYL_DASSERT_MSG(type() == SerializedType::Struct, "Attempted to cast non-struct type to Struct");
    return reinterpret_cast<const StructSerializedTypeInfo&>(*this);
}

ComponentInfo::ComponentInfo (std::string name, std::unique_ptr<ISerializedTypeInfo> typeInfo) :
    m_name{std::move(name)},
    m_typeInfo{std::move(typeInfo)} {}

std::string_view ComponentInfo::name () const noexcept {
    return m_name;
}

const ISerializedTypeInfo& ComponentInfo::typeInfo () const noexcept {
    return *m_typeInfo;
}

ComponentInfo ComponentInfo::GetInfo (std::string name, ISerializableBase& serializable) {
    SerializedTypeVisitor visitor{};
    visitor.visit(serializable);
    return ComponentInfo{std::move(name), std::move(visitor.info)};
}

PrimitiveSerializedTypeInfo::PrimitiveSerializedTypeInfo (PrimitiveType primType) : m_primitiveType{primType} {}

SerializedType PrimitiveSerializedTypeInfo::type () const noexcept {
    return SerializedType::Primitive;
}

PrimitiveType PrimitiveSerializedTypeInfo::primitiveType () const noexcept {
    return m_primitiveType;
}

ArraySerializedTypeInfo::ArraySerializedTypeInfo (std::unique_ptr<ISerializedTypeInfo> elementType) :
    m_elementType{std::move(elementType)} {}

SerializedType ArraySerializedTypeInfo::type () const noexcept {
    return SerializedType::Array;
}

const ISerializedTypeInfo& ArraySerializedTypeInfo::elementType () const noexcept {
    return *m_elementType;
}

MapSerializedTypeInfo::MapSerializedTypeInfo (std::unique_ptr<ISerializedTypeInfo> valueType) :
    m_valueType{std::move(valueType)} {}

SerializedType MapSerializedTypeInfo::type () const noexcept {
    return SerializedType::Map;
}

const ISerializedTypeInfo& MapSerializedTypeInfo::valueType () const noexcept {
    return *m_valueType;
}

AssetSerializedTypeInfo::AssetSerializedTypeInfo (std::string asset) : m_asset{std::move(asset)} {}

SerializedType AssetSerializedTypeInfo::type () const noexcept {
    return SerializedType::Asset;
}

std::string_view AssetSerializedTypeInfo::asset () const noexcept {
    return m_asset;
}

StructSerializedTypeInfo::StructSerializedTypeInfo (
    std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>> members) :
    m_members{std::move(members)} {}

SerializedType StructSerializedTypeInfo::type () const noexcept {
    return SerializedType::Struct;
}

const std::unordered_map<std::string, std::unique_ptr<ISerializedTypeInfo>>&
StructSerializedTypeInfo::members () const noexcept {
    return m_members;
}
