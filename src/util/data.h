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
        using iterator = std::vector<DataValue>::iterator;
        using const_iterator = std::vector<DataValue>::const_iterator;
        DataArray () = default;
        template <typename T>
        DataArray (std::vector<T>& _values);
        DataArray (std::vector<DataValue>& _values);
        DataArray (std::vector<DataValue>&& _values);

        std::size_t size ();

        DataValue& operator[] (std::size_t index);

        bool empty ();

        iterator begin ();
        iterator end ();

        const_iterator cbegin ();

        const_iterator cend ();

        void clear ();

        template <typename T>
        iterator insert (const_iterator pos, const T& val);

        template <typename T>
        iterator insert (const_iterator pos, T&& val);

        template <typename T, typename ...Args>
        iterator emplace (const_iterator pos, Args... args);

        iterator erase (const_iterator pos);

        template <typename ...Args>
        iterator emplace (const_iterator pos, Args... args);

        template <typename T>
        void push_back (const T& val);

        template <typename T>
        void push_back (T&& val);

        template <typename T, typename ...Args>
        iterator emplace_back (Args... args);
        template <typename ...Args>
        iterator emplace_back (Args... args);

        void pop_back ();


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

    template <typename T>
    DataArray::DataArray (std::vector<T>& _values) {
        for (auto i : _values) {
            values.emplace_back(i);
        }
    }

    template <typename T>
    DataArray::iterator DataArray::insert (const_iterator pos, const T& val) {
        return values.insert(pos, val);
    }

    template <typename T>
    DataArray::iterator DataArray::insert (const_iterator pos, T&& val) {
        return values.insert(pos, val);
    }

    template <typename T, typename ...Args>
    DataArray::iterator DataArray::emplace (const_iterator pos, Args... args) {
        return values.emplace(T(args...));
    }

    template <typename ...Args>
    DataArray::iterator DataArray::emplace (const_iterator pos, Args... args) {
        return values.emplace(args...);
    }

    template <typename T>
    void DataArray::push_back (const T& val) {
        values.push_back(val);
    }

    template <typename T>
    void DataArray::push_back (T&& val) {
        auto v = DataValue(val);
        values.push_back(std::move(v));
    }

    template <typename T, typename ...Args>
    DataArray::iterator DataArray::emplace_back (Args... args) {
        return values.push_back(T(args...));
    }

    template <typename ...Args>
    DataArray::iterator DataArray::emplace_back (Args... args) {
        return values.push_back(args...);
    }

    void testData ();
}
#endif
