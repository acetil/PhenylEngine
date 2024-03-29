#include <fstream>

#include "util/data.h"
#include "util/data_funcs.h"
#include "nlohmann/json.hpp"
#include "util/detail/loggers.h"


using namespace phenyl::util;

static phenyl::Logger LOGGER{"DATA", detail::UTIL_LOGGER};

namespace phenyl::util {
    void to_json (nlohmann::json& json, const DataValue& val);
    void to_json (nlohmann::json& json, const DataArray& val);
    void to_json (nlohmann::json& json, const DataObject& val);
    namespace internal {
        class DataObserver {
        public:
            static void toJsonValue (nlohmann::json& json, const DataValue& val) {
                json = std::visit([](const auto& v){
                    if constexpr(!std::is_same_v<decltype(v), const std::monostate&>) {
                        return nlohmann::json(v);
                    } else {
                        return nlohmann::json();
                    }
                }, val.obj);
            }
            static void toJsonObj (nlohmann::json& json, const DataObject& obj) {
                nlohmann::json jsonObj(nlohmann::json::value_t::object);
                for (auto [key, val] : obj.kv()) {
                    jsonObj[key] = nlohmann::json(val);
                }
                json = std::move(jsonObj);
            }
            static void toJsonArr (nlohmann::json& json, const DataArray& arr) {
                json = nlohmann::json(arr.values);
            }
        };
    }
}

DataValue internalParseJson (nlohmann::json& json);
DataObject internalParseJsonObject (nlohmann::json& json);
DataArray internalParseJsonArray (nlohmann::json& json);

DataObject::DataObject () = default;

DataObject::DataObject (DataValue& val) {
    DataObject obj;
    if (val.getValue(obj)) {
        values = std::move(obj.values);
    }
}

Map<std::string, DataValue>::iterator DataObject::begin () {
    return values.begin();
}

Map<std::string, DataValue>::const_iterator DataObject::begin () const {
    return values.begin();
}

Map<std::string, DataValue>::iterator DataObject::end () {
    return values.end();
}

Map<std::string, DataValue>::const_iterator DataObject::end () const {
    return values.end();
}

Map<std::string, DataValue>::const_iterator DataObject::cbegin () const {
    return values.cbegin();
}

Map<std::string, DataValue>::const_iterator DataObject::cend () const {
    return values.cend();
}

DataArray::DataArray (std::vector<DataValue>& _values) {
    values = _values;
}
DataArray::DataArray (std::vector<DataValue>&& _values) {
    values = std::move(_values);
}

std::size_t DataArray::size () const {
    return values.size();
}

DataValue& DataArray::operator[] (std::size_t index) {
    return values[index];
}

DataValue const& DataArray::operator[] (std::size_t index) const {
    return values[index];
}

bool DataArray::empty () {
    return values.empty();
}

DataArray::iterator DataArray::begin () {
    return values.begin();
}
DataArray::iterator DataArray::end () {
    return values.end();
}

DataArray::const_iterator DataArray::cbegin () {
    return values.cbegin();
}

DataArray::const_iterator DataArray::cend () {
    return values.cend();
}

void DataArray::clear () {
    values.clear();
}

DataArray::iterator DataArray::erase (const_iterator pos) {
    return values.erase(pos);
}

void DataArray::pop_back () {
    values.pop_back();
}

DataValue phenyl::util::parseJson (const std::string& jsonStr) {
    auto json = nlohmann::json::parse(jsonStr);
    return internalParseJson(json);
}

DataValue phenyl::util::parseFromFile (const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        PHENYL_LOGE(LOGGER, "Failed to read file: {}", filepath);
        return DataValue();
    }
    try {
        auto json = nlohmann::json::parse(file);
        return internalParseJson(json);
    } catch (std::exception&) {
        PHENYL_LOGE(LOGGER, "Failed to parse json from file {}", filepath);
    }
    return DataValue();
}

DataValue phenyl::util::parseFromStream (std::istream& stream) {
    try {
        auto json = nlohmann::json::parse(stream);
        return internalParseJson(json);
    } catch (std::exception&) {
        PHENYL_LOGE(LOGGER, "Failed to parse json from stream");
        return util::DataValue{};
    }
}

DataValue internalParseJson (nlohmann::json& json) {
    if (json.is_boolean()) {
        return DataValue(json.get<bool>());
    } else if (json.is_number_float()) {
        return DataValue(json.get<float>());
    } else if (json.is_number()) {
        return DataValue(json.get<int>());
    } else if (json.is_string()) {
        return DataValue(json.get<std::string>());
    } else if (json.is_object()) {
        return DataValue(std::move(internalParseJsonObject(json)));
    } else if (json.is_array()) {
        return DataValue(std::move(internalParseJsonArray(json)));
    } else {
        return DataValue();
    }
}

DataObject internalParseJsonObject (nlohmann::json& json) {
    DataObject obj;
    for (auto& [key, val] : json.items()) {
        obj[key] = std::move(internalParseJson(val));
    }
    return obj;
}

DataArray internalParseJsonArray (nlohmann::json& json) {
    DataArray arr;
    for (auto& i : json) {
        arr.emplace_back(std::move(internalParseJson(i)));
    }
    return arr;
}

std::string DataValue::convertToJson () {
    nlohmann::json json(*this);
    return json.dump();
}

std::string DataValue::convertToJsonPretty (int indent) {
    nlohmann::json json(*this);
    return json.dump(indent);
}

std::string DataArray::convertToJson () {
    nlohmann::json json(*this);
    return json.dump();
}

std::string DataArray::convertToJsonPretty (int indent) {
    nlohmann::json json(*this);
    return json.dump(indent);
}

std::string DataObject::convertToJson () {
    nlohmann::json json(*this);
    return json.dump();
}

std::string DataObject::convertToJsonPretty (int indent) {
    nlohmann::json json(*this);
    return json.dump(indent);
}

void phenyl::util::to_json (nlohmann::json& json, const DataValue& val) {
    internal::DataObserver::toJsonValue(json, val);
}
void phenyl::util::to_json (nlohmann::json& json, const DataArray& val) {
    internal::DataObserver::toJsonArr(json, val);
}
void phenyl::util::to_json (nlohmann::json& json, const DataObject& val) {
    internal::DataObserver::toJsonObj(json, val);
}

DataValue phenyl::util::phenyl_to_data (const DataValue& val) {
    return val;
}

bool phenyl::util::phenyl_from_data (const DataValue& dataVal, DataValue& val) {
    val = dataVal;
    return true;
}

phenyl::meta::type_list_unroll<std::variant, phenyl::util::data_types>& phenyl::util::internal::internalGetData (util::DataValue& val) {
    return val.obj;
}

const phenyl::meta::type_list_unroll<std::variant, data_types>& phenyl::util::internal::internalGetDataConst (const util::DataValue& val) {
    return val.obj;
}

/*util::DataValue phenyl_to_data (unsigned int& val) {
    unsigned int v = val;
    int v2 = *((int*)&v);
    util::DataValue d;
    d = v2;
    return d;
}

bool phenyl_from_data (const DataValue& val, unsigned int& out) {
    int v2;
    if (!val.getValue(v2)) {
        return false;
    }
    out = *((unsigned int*)&v2);
    return true;
}*/