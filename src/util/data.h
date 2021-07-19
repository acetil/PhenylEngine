#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

#include "meta.h"

#include "data_funcs.h"

#include "map.h"

#ifndef DATA_H
#define DATA_H
namespace util {
    class DataValue;
    class DataObject;

    class DataObject {
    private:
        Map<std::string, DataValue> values;
    public:
        DataObject();
        //DataObject(DataValue& val);
    };
    class DataArray {
    private:
        std::vector<DataValue> values;
    public:

    };

    using data_types = meta::type_list_wrapper<int, float, bool, std::string, DataObject, DataArray, std::monostate>;
    //using data_types = meta::type_list_wrapper<int, float, bool, std::string, DataObject, DataArray, std::monostate>;
    class DataValue {
    private:
        //std::variant<int, float, std::string, DataObject, DataArray> obj;
        meta::type_list_unroll<std::variant, data_types> obj{};
    public:
        DataValue() {
            obj = std::monostate{};
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        explicit DataValue (T& val) {
            obj = val;
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        explicit DataValue (T& val) {
            obj = std::move(todata(val).obj);
        }

        //template <typename T>
        //bool getValue (T& val);
        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        bool getValue  (T& val) {
            auto p = std::get_if<T>(&obj);
            if (p) {
                val = *p;
            }
            return p == nullptr;
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        bool getValue (T& val) {
            return fromdata<T>(*this, val);
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        DataValue& operator= (T val) {
            obj = val;
            return *this;
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        DataValue& operator= (T val) {
            obj = std::move(todata(val).obj);
            return *this;
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        operator T() {
            return std::get<T>(obj);
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
         operator T(){
             T val;
             if (fromdata(*this, val)) {
                 return val;
             } else {
                 throw std::bad_variant_access();
             }
        }
    };

    void testData ();
}
#endif
