#include <utility>

#include "common/string_serializer.h"


common::StringSerializer::StringSerializer (std::string indent) : indent{std::move(indent)} {}

bool common::StringSerializer::visit (bool val) {
    putIndent();
    sstream << val;
    return true;
}

bool common::StringSerializer::visit (std::string val) {
    putIndent();
    sstream << "\"" << val << "\"";
    return true;
}

bool common::StringSerializer::pushObject () {
    putIndent();
    pushIndent();
    sstream << "{\n";
    justAfterPush = true;
    return true;
}

void common::StringSerializer::popObject () {
    if (!justAfterPush) {
        sstream << "\n";
    }
    justAfterPush = false;
    popIndent();
    putIndent();
    sstream << "}";
}

bool common::StringSerializer::pushMember (const std::string& memberName) {
    if (justAfterPush) {
        justAfterPush = false;
    } else {
        sstream << ",\n";
    }

    putIndent();
    disableNextIndent();
    sstream << "\"" << memberName << "\"" << " : ";
    return true;
}

void common::StringSerializer::popMember () {

}

bool common::StringSerializer::pushList () {
    putIndent();
    sstream << "[\n";
    pushIndent();
    return false;
}

bool common::StringSerializer::listNext () {
    sstream << ",\n";
    return false;
}

void common::StringSerializer::popList () {
    popIndent();
    putIndent();
    sstream << "]";
}

void common::StringSerializer::putIndent () {
    for (auto i = 0; i < tempIndentLevel; i++) {
        sstream << indent;
    }

    tempIndentLevel = indentLevel;
}

void common::StringSerializer::disableNextIndent () {
    tempIndentLevel = 0;
}

void common::StringSerializer::pushIndent () {
    indentLevel++;
    tempIndentLevel = indentLevel;
}

void common::StringSerializer::popIndent () {
    assert(indentLevel > 0);
    indentLevel--;
    tempIndentLevel = indentLevel;
}

bool common::StringSerializer::pushArray (std::size_t size) {
    return pushList();
}

bool common::StringSerializer::arrayNext () {
    return listNext();
}

void common::StringSerializer::popArray () {
    popList();
}

bool common::StringSerializer::visit (std::int8_t val) {
    putIndent();
    sstream << val;
    return true;
}

bool common::StringSerializer::visit (std::int16_t val) {
    putIndent();
    sstream << val;
    return true;
}

bool common::StringSerializer::visit (std::int32_t val) {
    putIndent();
    sstream << val;
    return true;
}

bool common::StringSerializer::visit (std::int64_t val) {
    putIndent();
    sstream << val;
    return true;
}

bool common::StringSerializer::visit (float val) {
    putIndent();
    sstream << val;
    return true;
}
