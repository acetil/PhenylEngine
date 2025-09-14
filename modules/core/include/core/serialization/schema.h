#pragma once

#include "graphics/maths_headers.h"
#include "serializer.h"

namespace phenyl::core {
class ISerializableBase;

enum class EngineSerializableType {
    Entity,
    Prefab,
    Children
};

class ISchemaVisitor {
public:
    virtual ~ISchemaVisitor () = default;

    virtual void visitBool () = 0;

    virtual void visitInt8 () = 0;
    virtual void visitInt16 () = 0;
    virtual void visitInt32 () = 0;
    virtual void visitInt64 () = 0;

    virtual void visitUint8 () = 0;
    virtual void visitUint16 () = 0;
    virtual void visitUint32 () = 0;
    virtual void visitUint64 () = 0;

    virtual void visitFloat () = 0;
    virtual void visitDouble () = 0;

    virtual void visitString () = 0;

    virtual void visitArray (ISerializableBase& serializable) = 0;
    virtual void visitMap (ISerializableBase& serializable) = 0;

    virtual void pushStruct (std::string_view name) = 0;
    virtual void visitMember (std::string_view memberName, ISerializableBase& serializable) = 0;
    virtual void popStruct () = 0;

    virtual void visitEngineType (EngineSerializableType type) = 0;
    virtual void visitVector (ISerializableBase& type, glm::length_t N) = 0;
    virtual void visitMatrix (ISerializableBase& type, glm::length_t H, glm::length_t W) = 0;
    virtual void visitQuaternion () = 0;
    virtual void visitAsset (std::string_view assetName) = 0;

    virtual void visit (ISerializableBase& serializable) = 0;

    template <typename T>
    void visit () {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        visit(serializable);
    }

    template <SerializableType T>
    void visitArray () {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        visitArray(serializable);
    }

    template <SerializableType T>
    void visitMap () {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        visitMap(serializable);
    }

    template <SerializableType T>
    void visitMember (std::string_view memberName) {
        ISerializable<T>& serializable = detail::GetSerializable<T>();
        visitMember(memberName, serializable);
    }

    template <SerializableType T>
    void visitVector (glm::length_t N) {
        visitVector(detail::GetSerializable<T>(), N);
    }

    template <SerializableType T>
    void visitMatrix (glm::length_t H, glm::length_t W) {
        visitMatrix(detail::GetSerializable<T>(), H, W);
    }
};
} // namespace phenyl::core
