#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <typeinfo>

#include "meta.h"

#include "data_funcs.h"

#include "map.h"

#include "logging/logging.h"

namespace phenyl::util {

    class DataValue;
    class DataObject;
    class DataArray;
    using data_types = meta::type_list_wrapper<int, float, bool, std::string, DataObject, DataArray, std::monostate>;

    namespace internal {
        class DataObserver;
        meta::type_list_unroll<std::variant, data_types>& internalGetData (util::DataValue& val);
        const meta::type_list_unroll<std::variant, data_types>& internalGetDataConst (const util::DataValue& val);
        template <typename T>
        static constexpr bool is_any_convertible = meta::type_list_unroll<std::disjunction, meta::type_list_map<meta::apply<meta::flip_args<std::is_convertible>::val, T>::template val, util::data_types>>::value;

        template <typename T>
        static constexpr bool is_convertible_to_any = meta::type_list_unroll<std::disjunction, meta::type_list_map<meta::apply<std::is_convertible, T>::template val, util::data_types>>::value;

        template <class T, std::enable_if_t<is_any_convertible<T>, bool> = true>
        bool getDataValue (const util::DataValue& val, T& obj);

        template <class T, std::enable_if_t<!is_any_convertible<T>, bool> = true>
        bool getDataValue (const util::DataValue& val, T& obj);

        //std::string& getDataValue (util::DataValue& val);

        //const std::string& getDataValue (const util::DataValue& val);
    }

    class DataObject {
    private:
        Map<std::string, DataValue> values;

    public:
        DataObject();
        explicit DataObject(DataValue& val);

        operator DataValue() const;

        Map<std::string, DataValue>::iterator begin ();
        Map<std::string, DataValue>::const_iterator begin () const;
        Map<std::string, DataValue>::iterator end ();
        Map<std::string, DataValue>::const_iterator end () const;

        Map<std::string, DataValue>::const_iterator cbegin () const;
        Map<std::string, DataValue>::const_iterator cend () const;

        Map<std::string, DataValue>::kv_view kv () {
            return values.kv();
        }

        Map<std::string, DataValue>::const_kv_view kv () const {
            return values.kv();
        }

        template <typename K>
        DataValue& operator[] (const K& key);

        template <typename K>
        bool contains (const K& key) const;

        template <typename T, typename K>
        bool contains (const K& key) const;

        template <typename K>
        DataValue& at (const K& key);
        template <typename T, typename K>
        auto at (const K& key);

        template <typename K>
        const DataValue& at (const K& key) const;

        template <typename T, typename K>
        auto at (const K& key) const;

        std::string convertToJson ();
        std::string convertToJsonPretty (int indent = 4);

        std::string toString () const;

        bool empty () const {
            return values.empty();
        }

        friend class internal::DataObserver;
    };
    class DataArray {
    private:
        std::vector<DataValue> values;
    public:
        using iterator = std::vector<DataValue>::iterator;
        using reference = std::vector<DataValue>::reference;
        using const_iterator = std::vector<DataValue>::const_iterator;
        DataArray () = default;
        template <typename T>
        DataArray (std::vector<T>& _values);
        DataArray (std::vector<DataValue>& _values);
        DataArray (std::vector<DataValue>&& _values);

        std::size_t size () const;

        DataValue& operator[] (std::size_t index);

        DataValue const& operator[] (std::size_t index) const;

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
        reference emplace (const_iterator pos, Args... args);

        iterator erase (const_iterator pos);

        template <typename ...Args>
        reference
        emplace (const_iterator pos, Args... args);

        template <typename T>
        void push_back (const T& val);

        template <typename T>
        void push_back (T&& val);

        template <typename T, typename ...Args>
        reference emplace_back (Args... args);
        template <typename ...Args>
        reference emplace_back (Args... args);

        void pop_back ();

        std::string convertToJson ();
        std::string convertToJsonPretty (int indent = 4);

        std::string toString () const;

        friend class internal::DataObserver;
    };

    //using data_types = meta::type_list_wrapper<float, bool, std::string, DataObject, DataArray, std::monostate, int>;
    //using data_types = meta::type_list_wrapper<int, float, bool, std::string, DataObject, DataArray, std::monostate>;
    class DataValue {
    private:
        //std::variant<int, float, std::string, DataObject, DataArray> obj;
        meta::type_list_unroll<std::variant, data_types> obj{};
        template <typename T>
        void setObj (T&& v) {
            obj = v;
        }


    public:
        DataValue() {
            static_assert(std::is_same_v<DataObject, meta::get_nth_typelist<4, data_types>>);
            obj = std::monostate{};
        }
        ~DataValue() = default;

        // Explicit implementations are necessary because c++ is dumb
        DataValue (DataValue const& other) {
            obj = other.obj;
        }; // Both are necessary because c++ is dumb

        DataValue& operator= (DataValue const& other) {
            obj = other.obj;

            return *this;
        };
        DataValue& operator= (DataValue&& other)  noexcept {
            obj = std::move(other.obj);

            return *this;
        };

        DataValue (DataValue&& other) {
            obj = std::move(other.obj);
        };



        /*template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        explicit DataValue (T val) {
            obj = val;
        }*/

        template <typename T, std::enable_if_t<meta::is_in_typelist<std::remove_cvref_t<T>, data_types>, bool> = true>
        explicit DataValue (T&& val) {
            setObj(std::forward<T>(val));
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<std::remove_cvref_t<T>, data_types>, bool> = true>
        explicit DataValue (T&& val) {
            obj = std::move(phenyl_to_data(val).obj);
        }

        explicit DataValue (const char* str) {
            obj = std::string{str};
        }

        //template <typename T>
        //bool getValue (T& val);
        /*template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        bool getValue  (T& val) const {
            auto p = std::get_if<T>(&obj);
            if (p) {
                val = *p;
            }
            return p != nullptr;
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        bool getValue (T& val) {
            return phenyl_from_data<T>(*this, val);
        }*/
        template <typename T>
        bool getValue (T& val) const {
            return internal::getDataValue<T>(*this, val);
        }


        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        DataValue& operator= (const T& val) {
            obj = val;
            return *this;
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        DataValue& operator= (T&& val) {
            obj = std::forward<T>(val);
            return *this;
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        DataValue& operator= (const T& val) {
            obj = std::move(phenyl_to_data(val).obj);
            return *this;
        }

        DataValue& operator= (unsigned int val) {
            obj = *((int*)&val);
            return *this;
        }

        template <typename T, std::enable_if_t<!(std::is_same_v<T, DataValue> || std::is_same_v<T, DataObject> || std::is_same_v<T, DataArray>), bool> = true>
        explicit operator T() const {
            return get<T>();
        }

        template <typename T, std::enable_if_t<std::is_same_v<T, DataObject> || std::is_same_v<T, DataArray>, bool> = true>
        operator T() const {
            return std::get<T>(obj);
        }

        /*template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types>, bool> = true>
        explicit operator T(){
             T val;
             if (phenyl_from_data(*this, val)) {
                 return val;
             } else {
                 throw std::bad_variant_access();
             }
        }*/

        /*operator unsigned int () {
            int val;
            if (phenyl_from_data(*this, val)) {
                return *((unsigned int*)&val);
            } else {
                throw std::bad_variant_access();
            }
        }*/

        /*template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types> && !std::is_same_v<T, float>, bool> = true>
        T& get () {
            return std::get<T>(obj);
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types> && !std::is_same_v<T, float>, bool> = true>
        T const& get () const {
            return std::get<T>(obj);
        }

        template <typename T, std::enable_if_t<!meta::is_in_typelist<T, data_types> || std::is_same_v<T, float>, bool> = true>
        T get () const {
            T val;
            if (phenyl_from_data(*this, val)) {
                return val;
            } else {
                throw std::bad_variant_access();
            }
        }

        //template<>
        unsigned int get () const {
            return *((unsigned int*)&std::get<int>(obj));
        }

        //template<>
        [[nodiscard]] float get<float> () const {
            if (is<float>()) {
                return std::get<float>(obj);
            } else if (is<int>()) {
                return (float)std::get<int>(obj);
            } else {
                throw std::bad_variant_access();
            }
        }*/

        template <typename T, std::enable_if_t<!(std::is_same_v<T, DataObject> || std::is_same_v<T, DataArray> || std::is_same_v<T, std::string>), bool> = true>
        T get () const {
            T val;

            if (internal::getDataValue<T>(*this, val)) {
                return val;
            } else {
                throw std::bad_variant_access();
            }
        }

        template <typename T, std::enable_if_t<std::is_same_v<T, DataObject> || std::is_same_v<T, DataArray> || std::is_same_v<T, std::string>, bool> = true>
        T& get () {
            return std::get<T>(obj);
        }

        template <typename T, std::enable_if_t<std::is_same_v<T, DataObject> || std::is_same_v<T, DataArray> || std::is_same_v<T, std::string>, bool> = true>
        const T& get () const {
            return std::get<T>(obj);
        }

        template <typename T, std::enable_if_t<meta::is_in_typelist<T, data_types>, bool> = true>
        bool is () const {
            return std::visit([](const auto& v) {
                return std::is_same_v<decltype(v), const T&>;
            }, obj);
        }


        bool empty () const {
            return std::visit([](const auto& val) {return std::is_same_v<decltype(val), const std::monostate&>;}, obj);
        }

        std::string toString () const {
            return std::visit([](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    return std::string("empty");
                } else if constexpr (std::is_same_v<T, DataArray>) {
                    return ((DataArray&&)arg).toString();
                } else if constexpr (std::is_same_v<T, DataObject>) {
                    return ((DataObject&&)arg).toString();
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return (std::string)arg;
                } else {
                    return std::to_string(arg);
                }
            }, obj);
        }

        std::string convertToJson ();
        std::string convertToJsonPretty (int indent = 4);

        template<class T>
        friend bool operator== (DataValue& v1, const T& v2);
        template<class T>
        friend bool operator!= (DataValue& v1, const T& v2);
        template<class T>
        friend bool operator< (DataValue& v1, const T& v2);
        template<class T>
        friend bool operator> (DataValue& v1, const T& v2);
        template<class T>
        friend bool operator<= (DataValue& v1, const T& v2);
        template<class T>
        friend bool operator>= (DataValue& v1, const T& v2);

        friend class internal::DataObserver;
        friend meta::type_list_unroll<std::variant, data_types>& internal::internalGetData (util::DataValue& val);
        friend const meta::type_list_unroll<std::variant, data_types>& internal::internalGetDataConst (const util::DataValue& val);
    };

    inline DataObject::operator DataValue() const {
        return DataValue(*this);
    }

    template<typename K>
    DataValue& DataObject::operator[] (const K& key) {
        return values[key];
    }


    template <typename K>
    bool DataObject::contains (const K& key) const {
        return values.contains(key);
    }

    template <typename T, typename K>
    bool DataObject::contains (const K& key) const {
        return values.contains(key) && values.at(key).template is<T>();
    }

    template <typename K>
    DataValue& DataObject::at (const K& key) {
        return values.at(key);
    }

    template <typename T, typename K>
    auto DataObject::at (const K& key) {
        return values.at(key).template get<T>();
    }

    template <typename K>
    const DataValue& DataObject::at (const K& key) const {
        return values.at(key);
    }

    template <typename T, typename K>
    auto DataObject::at (const K& key) const {
        return values.at(key).template get<T>();
    }

    inline std::string DataObject::toString () const {
        std::stringstream stream;
        stream << "{";
        bool first = true;
        for (auto [key, val] : values.kv()) {
            if (first) {
                first = false;
            } else {
                stream << ", ";
            }

            stream << key << " : " << val.toString();
        }
        stream << "}";
        return stream.str();
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
    DataArray::reference DataArray::emplace (const_iterator pos, Args... args) {
        return values.emplace(T(args...));
    }

    template <typename ...Args>
    DataArray::reference DataArray::emplace (const_iterator pos, Args... args) {
        return values.emplace(args...);
    }

    template <typename T>
    void DataArray::push_back (const T& val) {
        values.push_back(util::DataValue(val));
    }

    template <typename T>
    void DataArray::push_back (T&& val) {
        auto v = DataValue(std::forward<T>(val));
        values.push_back(std::move(v));
    }

    template <typename T, typename ...Args>
    DataArray::reference DataArray::emplace_back (Args... args) {
        return values.emplace_back(T(args...));
    }

    template <typename ...Args>
    DataArray::reference DataArray::emplace_back (Args... args) {
        return values.emplace_back(args...);
    }

    inline std::string DataArray::toString () const{
        std::stringstream stream;
        stream << "[";
        bool first = true;

        for (auto& i : values) {
            if (first) {
                first = false;
            } else {
                stream << ", ";
            }

            stream << i.toString();
        }
        stream << "]";
        return stream.str();
    };

    void testData ();

    namespace internal {
        template<template<typename ...> typename Cond, typename F, typename T, bool defaultVal = false>
        struct OperatorVisitor {
        private:
            F func;
            const T& t;
        public:
            OperatorVisitor (F f, const T& _t) : func{f}, t{_t} {}

            template<typename V>
            auto operator() (
                    const V& v) -> std::enable_if_t<meta::does_exist<Cond, meta::maybe_boxed<T>, meta::maybe_boxed<V>>, bool> {
                return func(t, v);
            }

            template<typename V>
            auto operator() (
                    const V& v) -> std::enable_if_t<!meta::does_exist<Cond, meta::maybe_boxed<T>, meta::maybe_boxed<V>>, bool> {
                return defaultVal;
            }
        };

        struct Operations {
        private:
            template<typename A, typename B>
            using eq_cond = decltype(std::declval<A>().operator==(std::declval<B>()));
            template<typename A, typename B>
            using neq_cond = decltype(std::declval<A>().operator!=(std::declval<B>()));
            template<typename A, typename B>
            using less_cond = decltype(std::declval<A>().operator<(std::declval<B>()));
            template<typename A, typename B>
            using more_cond = decltype(std::declval<A>().operator>(std::declval<B>()));
            template<typename A, typename B>
            using leq_cond = decltype(std::declval<A>().operator<=(std::declval<B>()));
            template<typename A, typename B>
            using geq_cond = decltype(std::declval<A>().operator>=(std::declval<B>()));

            template<template<typename ...> typename Cond, bool defaultVal = false, typename F, typename T>
            static auto get_visitor (F f, const T& t) {
                return OperatorVisitor<Cond, F, T, defaultVal>(f, t);
            }

        public:
            template<typename T>
            static auto eq (const T& t) {
                return get_visitor<eq_cond>([] (const auto& a, const auto& b) { return a == b; }, t);
            }

            template<typename T>
            static auto neq (const T& t) {
                return get_visitor<neq_cond, true>([] (const auto& a, const auto& b) { return a != b; }, t);
            }

            template<typename T>
            static auto less (const T& t) {
                return get_visitor<less_cond>([] (const auto& a, const auto& b) { return a < b; }, t);
            }

            template<typename T>
            static auto more (const T& t) {
                return get_visitor<more_cond>([] (const auto& a, const auto& b) { return a > b; }, t);
            }

            template<typename T>
            static auto leq (const T& t) {
                return get_visitor<leq_cond>([] (const auto& a, const auto& b) { return a <= b; }, t);
            }

            template<typename T>
            static auto geq (const T& t) {
                return get_visitor<geq_cond>([] (const auto& a, const auto& b) { return a >= b; }, t);
            }
        };
    }

    // if op(x, y) is undefined then op(DataVal(x), y) = false, except for
    // op = !=, where its = true
    // Non-equality operators are swapped because Operations is from the perspective of v2
    template <typename T>
    bool operator== (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::eq(v2), v1.obj);
    }
    template <typename T>
    bool operator!= (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::neq(v2), v1.obj);
    }
    template <typename T>
    bool operator< (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::more(v2), v1.obj);
    }
    template <typename T>
    bool operator> (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::less(v2), v1.obj);
    }
    template <typename T>
    bool operator<= (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::geq(v2), v1.obj);
    }
    template <typename T>
    bool operator>= (DataValue& v1, const T& v2) {
        return std::visit(internal::Operations::leq(v2), v1.obj);
    }

    DataValue parseJson (const std::string& json);
    DataValue parseFromFile (const std::string& filepath);
    DataValue parseFromStream (std::istream& stream);

    namespace internal {
        template <class T, std::enable_if_t<is_any_convertible<T>, bool>>
        bool getDataValue (const util::DataValue& val, T& obj) {
            const auto& data = internalGetDataConst(val);

            return std::visit([&obj](const auto& v) {
                    if constexpr (std::is_same_v<std::remove_cvref<decltype(v)>, T>) {
                        obj = v;
                        return true;
                    } else if constexpr (std::is_convertible_v<decltype(v), T>) {
                        obj = (T)v;
                        return true;
                    } else {
                        return false;
                    }
                }, data);
        }

        template <class T, std::enable_if_t<!is_any_convertible<T>, bool>>
        bool getDataValue (const util::DataValue& val, T& obj) {
            return phenyl_from_data(val, obj);
        }

        /*std::string& getDataValue (util::DataValue& val) {
            auto& data = internalGetDataConst(val);
            return std::visit([] (auto& v) {
                if constexpr (std::is_same_v<std::remove_cvref_t<decltype(v)>, std::string>) {
                    return v;
                } else {
                    throw std::bad_variant_access();
                }
            }, data);
        }

        const std::string& getDataValue<std::string> (const util::DataValue& val) {
            const auto& data = internalGetDataConst(val);
            return std::visit([] (const auto& val) {
                if constexpr (std::is_same_v<std::remove_cvref_t<decltype(val), std::string>) {
                    return val;
                } else {
                    throw std::bad_variant_access();
                }
            }, data);
        }*/
    }
}
