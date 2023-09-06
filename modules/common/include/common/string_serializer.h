#pragma once

#include "serializer.h"

#include <sstream>

namespace common {
    class StringSerializer : public SerializeVisitor {
    private:
        std::stringstream sstream;
        std::string indent;
        int indentLevel{0};
        int tempIndentLevel{0};
        bool justAfterPush = false;
        void putIndent ();
        void disableNextIndent ();
        void pushIndent ();
        void popIndent ();
    protected:
        bool pushMember(const std::string &memberName) override;
        void popMember() override;

        bool pushList() override;
        bool listNext() override;
        void popList() override;

        bool pushArray(std::size_t size) override;
        bool arrayNext() override;
        void popArray() override;
    public:
        explicit StringSerializer (std::string indent="\t");
        bool pushObject() override;
        void popObject() override;

        template <Serializable T>
        std::string serialize (const T& val) {
            assert(SerializeVisitor::visit(val));
            std::string str = std::move(sstream.str());
            sstream = std::stringstream{};
            assert(indentLevel == 0);
            assert(tempIndentLevel == 0);
            assert(!justAfterPush);

            return std::move(str);
        }

        bool visit(bool val) override;
        bool visit(std::string val) override;

        bool visit(std::int8_t val) override;
        bool visit(std::int16_t val) override;
        bool visit(std::int32_t val) override;
        bool visit(std::int64_t val) override;

        bool visit(float val) override;
    };
}