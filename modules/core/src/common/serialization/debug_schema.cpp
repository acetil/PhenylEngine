#include "core/serialization/debug_schema.h"

using namespace phenyl::core;

DebugSchemaVisitor::DebugSchemaVisitor (std::ostream& stream) : m_stream{stream} {}

void DebugSchemaVisitor::visitBool () {
    m_stream << "bool";
}

void DebugSchemaVisitor::visitInt8 () {
    m_stream << "int8";
}

void DebugSchemaVisitor::visitInt16 () {
    m_stream << "int16";
}

void DebugSchemaVisitor::visitInt32 () {
    m_stream << "int32";
}

void DebugSchemaVisitor::visitInt64 () {
    m_stream << "int64";
}

void DebugSchemaVisitor::visitUint8 () {
    m_stream << "uint8";
}

void DebugSchemaVisitor::visitUint16 () {
    m_stream << "uint16";
}

void DebugSchemaVisitor::visitUint32 () {
    m_stream << "uint32";
}

void DebugSchemaVisitor::visitUint64 () {
    m_stream << "uint64";
}

void DebugSchemaVisitor::visitFloat () {
    m_stream << "float";
}

void DebugSchemaVisitor::visitDouble () {
    m_stream << "double";
}

void DebugSchemaVisitor::visitString () {
    m_stream << "string";
}

void DebugSchemaVisitor::visitArray (ISerializableBase& serializable) {
    m_stream << "[";
    visit(serializable);
    m_stream << "]";
}

void DebugSchemaVisitor::visitMap (ISerializableBase& serializable) {
    m_stream << "map<string, ";
    visit(serializable);
    m_stream << ">";
}

void DebugSchemaVisitor::pushStruct (std::string_view name) {
    m_stream << name << "{\n";
    m_indentLevel++;
}

void DebugSchemaVisitor::visitMember (std::string_view memberName, ISerializableBase& serializable) {
    printIndent();
    m_stream << memberName << ": ";
    visit(serializable);
    m_stream << ",\n";
}

void DebugSchemaVisitor::popStruct () {
    m_indentLevel--;
    printIndent();
    m_stream << "}";
}

void DebugSchemaVisitor::visitEngineType (EngineSerializableType type) {
    switch (type) {
    case EngineSerializableType::Entity:
        m_stream << "Entity";
        break;
    case EngineSerializableType::Prefab:
        m_stream << "Prefab";
        break;
    case EngineSerializableType::Children:
        m_stream << "Children";
        break;
    default:
        PHENYL_ABORT("Unexpected EngineSerializableType: {}", static_cast<std::uint32_t>(type));
    }
}

void DebugSchemaVisitor::visitVector (ISerializableBase& type, glm::length_t N) {
    m_stream << std::format("vec{}<", N);
    visit(type);
    m_stream << ">";
}

void DebugSchemaVisitor::visitMatrix (ISerializableBase& type, glm::length_t H, glm::length_t W) {
    m_stream << std::format("mat{}x{}<", H, W);
    visit(type);
    m_stream << ">";
}

void DebugSchemaVisitor::visitQuaternion () {
    m_stream << "Quaternion";
}

void DebugSchemaVisitor::visitAsset (std::string_view assetName) {
    m_stream << std::format("Asset<{}>", assetName);
}

void DebugSchemaVisitor::visit (ISerializableBase& serializable) {
    serializable.accept(*this);
}

void DebugSchemaVisitor::printIndent () {
    for (auto i = 0u; i < m_indentLevel; i++) {
        m_stream << "\t";
    }
}
