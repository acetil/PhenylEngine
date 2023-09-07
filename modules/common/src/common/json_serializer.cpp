#include <nlohmann/json.hpp>

#include "common/json_serializer.h"

using namespace common;

namespace {
    class NlohmannDeserializeVisitor : public JSONDeserializeVisitor {
    private:
        nlohmann::json json{};
        nlohmann::json_pointer<std::string> jsonPtr{};

        std::string advanceList (const std::string& index) {
            auto i = std::stoi(index);
            i++;
            return std::to_string(i);
        }

        bool needIndex = false;
    protected:
        bool openJson (const std::string& jsonStr) override {
            try {
                json = nlohmann::json::parse(jsonStr);
            } catch (const nlohmann::json::parse_error& error) {
                logging::log(LEVEL_ERROR, "nlohmann JSON parse error: {}", error.what());
                return false;
            }

            return true;
        }

        void closeJson () override {
            json.clear();
            assert(jsonPtr.empty());
        }

        bool pushMember (const std::string& memberName) override {
            if (!json.contains(jsonPtr) || !json.at(jsonPtr).contains(memberName)) {
                logging::log(LEVEL_DEBUG, "Expected member {}!", memberName);
                return false;
            }
            jsonPtr.push_back(memberName);
            return true;
        }

        void popMember () override {
            jsonPtr.pop_back();
        }

        bool pushList () override {
            if (!json.contains(jsonPtr) || !json.at(jsonPtr).is_array()) {
                return false;
            }
            needIndex = true;
            return true;
        }

        bool listNext () override {
            if (needIndex) {
                jsonPtr.push_back("0");
            } else {
                auto nextIndex = advanceList(jsonPtr.back());
                jsonPtr.pop_back();
                jsonPtr.push_back(nextIndex);
            }

            return json.contains(jsonPtr);
        }

        void popList () override {
            jsonPtr.pop_back();
        }

        bool pushArray (std::size_t size) override {
            if (!json.contains(jsonPtr) || !json.at(jsonPtr).is_array() || json.at(jsonPtr).get<nlohmann::json::array_t>().size() != size) {
                return false;
            }

            return pushList();
        }

        bool arrayNext() override {
            return listNext();
        }

        void popArray() override {
            jsonPtr.pop_back();
        }
    public:
        bool pushObject () override {
            return json.contains(jsonPtr) && json.at(jsonPtr).is_object();
        }

        void popObject () override {

        }

        bool visit (bool& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_boolean()) {
                val = v.get<bool>();
                return true;
            } else {
                return false;
            }
        }
        bool visit (std::string& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_string()) {
                val = v.get<std::string>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::int8_t& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_integer()) {
                val = v.get<std::int8_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit(std::uint8_t &val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_unsigned()) {
                val = v.get<std::uint8_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::int16_t& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_integer()) {
                val = v.get<std::int16_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::uint16_t &val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_unsigned()) {
                val = v.get<std::uint16_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::int32_t& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_integer()) {
                val = v.get<std::int32_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::uint32_t &val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_unsigned()) {
                val = v.get<std::uint32_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::int64_t& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_integer()) {
                val = v.get<std::int64_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (std::uint64_t &val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number_unsigned()) {
                val = v.get<std::uint64_t>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (float& val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number()) {
                val = v.get<float>();
                return true;
            } else {
                return false;
            }
        }

        bool visit (double &val) override {
            if (!json.contains(jsonPtr)) {
                return false;
            }

            auto& v = json.at(jsonPtr);

            if (v.is_number()) {
                val = v.get<double>();
                return true;
            } else {
                return false;
            }
        }
    };
}

std::unique_ptr<JSONDeserializeVisitor> common::JSONDeserializeVisitor::Make () {
    return std::make_unique<NlohmannDeserializeVisitor>();
}
