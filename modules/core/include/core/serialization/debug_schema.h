#pragma once
#include "schema.h"

namespace phenyl::core {
class DebugSchemaVisitor : public ISchemaVisitor {
public:
    DebugSchemaVisitor (std::ostream& stream);

    void visitBool () override;

    void visitInt8 () override;
    void visitInt16 () override;
    void visitInt32 () override;
    void visitInt64 () override;

    void visitUint8 () override;
    void visitUint16 () override;
    void visitUint32 () override;
    void visitUint64 () override;

    void visitFloat () override;
    void visitDouble () override;

    void visitString () override;

    void visitArray (ISerializableBase& serializable) override;
    void visitMap (ISerializableBase& serializable) override;

    void pushStruct (std::string_view name) override;
    void visitMember (std::string_view memberName, ISerializableBase& serializable) override;
    void popStruct () override;

    void visitEngineType (EngineSerializableType type) override;
    void visitVector (ISerializableBase& type, glm::length_t N) override;
    void visitMatrix (ISerializableBase& type, glm::length_t H, glm::length_t W) override;
    void visitQuaternion () override;
    void visitAsset (std::string_view assetName) override;

    void visit (ISerializableBase& serializable) override;

private:
    std::ostream& m_stream;
    std::uint32_t m_indentLevel = 0;

    void printIndent ();
};
} // namespace phenyl::core
