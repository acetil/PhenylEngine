#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

#include "meta.h"

#include "data_funcs.h"

#include "map.h"

#ifndef UTIL_DATA_H
#define UTIL_DATA_H
namespace util {
    class DataValue;

    class DataObject {
    private:
        Map<std::string, DataValue> values;

    public:
        DataObject();
        explicit DataObject(DataValue& val);

        Map<std::string, DataValue>::iterator begin ();
        Map<std::string, DataValue>::iterator end ();

        Map<std::string, DataValue>::const_iterator cbegin ();
        Map<std::string, DataValue>::const_iterator cend ();

        template <typename T>
        DataValue& operator[] (const T& key);

        template <typename T>
        bool contains (const T& key);
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
            static_assert(std::is_same_v<DataObject, meta::get_nth_typelist<4, data_types>>);
            obj = std::monostate{};
        }

        DataValue (DataValue& other) = default;
        DataValue (DataValue const& other) = default; // Both are necessary because c++ is dumb

        DataValue& operator= (DataValue const& other) = default;
        DataValue& operator= (DataValue&& other) = default;

        DataValue (DataValue&& other) = default;

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
            return p != nullptr;
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

    template<typename T>
    DataValue& DataObject::operator[] (const T& key) {
        return values[key];
    }

    template <typename T>
    bool DataObject::contains (const T& key) {
        return values.contains(key);
    }
    void testData ();
}
#endif
