#pragma once

#include <type_traits>
#include <functional>
#include <tuple>

namespace meta {
    template <typename T>
    using add_pointer = T*;
    template <typename T>
    using remove_pointer = typename std::remove_pointer<T>::type;

    template <typename C, typename ...Args>
    struct get_first_impl {
        using type = C;
    };
    template <typename ...Args>
    using get_first = typename get_first_impl<Args...>::type;

    template<typename ...T>
    struct type_list {};

    template <typename ...T>
    struct type_list_wrapper {
        using args = type_list<T...>;
    };

    template <template <typename ...> typename T, typename L, typename = typename L::args>
    struct type_list_unroll_impl;

    template <template <typename ...> typename T, typename L, typename ...Args>
    struct type_list_unroll_impl <T, L, type_list<Args...>> {
        using val = T<Args...>;
    };

    template <template <typename ...> typename T, typename L>
    using type_list_unroll = typename type_list_unroll_impl<T, L>::val;

    template <typename T, typename L, typename = typename L::args>
    struct add_front_impl;

    template <typename T, typename L, typename ...Args>
    struct add_front_impl <T, L, type_list<Args...>> {
        using val = type_list<T, Args...>;
    };

    template <typename T, typename L>
    using list_add_front = typename add_front_impl<T, L>::val;


    template <template <typename ...> typename T, typename ...Args1>
    struct type_forward_impl {
        template <typename ...Args>
        using val = T<Args1..., Args...>;
    };

    template <template <typename ...> typename T, typename ...Args1>
    using type_forward = typename type_forward_impl<T, Args1...>::val;

    // The below can_call from https://stackoverflow.com/a/22882504
    struct can_call_test {
    public:
        template<typename F, typename... A>
        static decltype(std::declval<F>()(std::declval<A>()...), std::true_type())
        f(int);

        template<typename F, typename... A>
        static std::false_type
        f(...);
    };

    template <typename F, typename ...Args>
    class can_call_impl {
    public:
        using val = decltype(can_call_test::f<F, Args...>(0));
    };
    template<typename F, typename... A>
    using can_call = typename can_call_impl<F, A...>::val;

    template<typename ...T, typename F>
    constexpr bool is_callable (F&&) { return can_call<F, T...>{}; }


    template <typename F, typename... Args>
    struct is_callable_list_impl  {
        using val = can_call<F, Args...>;
    };


    template <typename F, typename L>
    constexpr bool is_callable_list (F&&) { return type_list_unroll<can_call, list_add_front<F, L>>{};}

    template <typename L>
    using type_list_tuple = type_list_unroll<std::tuple, L>;

    template <int N, typename ...Args>
    struct get_nth_type_impl;

    template <typename T, typename ...Args>
    struct get_nth_type_impl<0, T, Args...> {
        using val = T;
    };

    template <int N, typename T, typename ...Args>
    struct get_nth_type_impl<N, T, Args...> {
        using val = typename get_nth_type_impl<N - 1, Args...>::val;
    };

    template <int N, typename ...Args>
    using get_nth_type = typename get_nth_type_impl<N, Args...>::val;

    template <typename ...Args>
    struct get_nth_typelist_impl {
        template <int N>
        using val = get_nth_type<N, Args...>;
    };

    template <int N, typename L>
    using get_nth_typelist = typename type_list_unroll<get_nth_typelist_impl, L>::template val<N>;

    /*template <typename ...Args>
    struct pack_len_impl {
        constexpr int packlen = sizeof...(Args);
    };*/

    template <typename ...Args>
    struct pack_len_impl {
        static constexpr int val = sizeof...(Args);
    };

    template <typename ...Args>
    inline constexpr int pack_len = pack_len_impl<Args...>::val;

    //template <typename ...Args>
    //inline constexpr int tl_len<typename L<Args...>
    /*template <typename L, typename ...Args>
    constexpr int tl_len2 () {
        return pack_len<Args...>;
    }
    template <typename L, typename Args = typename L::args>
    constexpr int tl_len () {
        return tl_len2<L, Args>();
    };*/

    //template <typename L, typename ...Args>
    //inline constexpr int tl_len = pack_len<Args...>
    //template <typename ...Args>
    //int pack_len = pack_len_impl<Args...>::packlen;

    template <typename L>
    static constexpr int typelist_len = type_list_unroll<pack_len_impl, L>::val;

    /*template <typename L>
    constexpr int typelist_len = tl_len<L>();*/

    template <int N, typename T, typename L>
    struct is_in_impl;

    template <typename T, typename L>
    struct is_in_impl <0, T, L> {
        using val = typename std::false_type;
    };

    template <int N, typename T, typename L>
    struct is_in_impl  {
        //static_assert(N < 3);
        //static_assert(!std::is_same<get_nth_typelist<6, L>, double>());
        using val = typename std::disjunction<std::is_same<get_nth_typelist<N - 1, L>, T>, typename is_in_impl<N - 1, T, L>::val>;
    };

    template <typename T, typename L>
    inline constexpr bool is_in_typelist = typename is_in_impl<typelist_len<L>, T, L>::val();

    template <typename T>
    struct make_const_impl {
        using val = const T;
    };
    template <template <typename ...> typename T>
    struct make_const_impl2 {
        template <typename ...Args>
        using val = const T<Args...>;
    };

    template <typename T>
    using make_const = typename make_const_impl<T>::val;

    template <template <typename ...> typename T>
    using make_const2 = typename make_const_impl2<T>::val;

    template <typename T, typename = void>
    struct remove_const_if_exist_impl {
        using val = T;
    };

    template <typename T>
    struct remove_const_if_exist_impl <T, std::enable_if_t<std::is_const_v<T>, void>> {
        using val = typename std::remove_const<T>::type;
    };

    template <typename T>
    using remove_const_if_exist = typename remove_const_if_exist_impl<T>::val;



    template <template <typename ...> typename T, typename ...Args>
    struct pack_map_impl {
        using val = type_list_wrapper<T<Args>...>;
    };

    template <template <typename ...> typename T, typename ...Args>
    using pack_map = typename pack_map_impl<T, Args...>::val;

    //template <template <typename ...> typename T, typename L>

    template <template <typename ...> typename T, typename L, typename = typename L::args>
    struct type_list_map_impl;

    template <template <typename ...> typename T, typename L, typename ...Args>
    struct type_list_map_impl <T, L, type_list<Args...>> {
        using val = pack_map<T, Args...>;
    };


    template <template <typename ...> typename T, typename L>
    using type_list_map = typename type_list_map_impl<T, L>::val;


    struct does_exist_impl {
    private:
        template <typename ...Args>
        struct inner {
            // I hate this language so much this is so dumb
            template<template<typename ...> typename Template>
            static constexpr char SFINAE (Template<Args...>* t) { return 'a'; };

            template <template <typename ...> typename Template>
            static constexpr short SFINAE (...) {return 0;};
        };
    public:
        template <template <typename ...> typename Template, typename ...Args>
        static constexpr bool exists = sizeof(inner<Args...>::template SFINAE<Template>(nullptr)) == sizeof(char);
    };
    template <template <typename ...> typename Template, typename ...Args>
    static constexpr bool does_exist = does_exist_impl::exists<Template, Args...>;

    struct does_func_exist_impl {
    private:
        template <typename F, typename ...Args>
        static constexpr auto SFINAE (char* t) -> std::enable_if_t<std::is_same_v<decltype(std::declval<F>()(std::declval<Args>()...)),
                decltype(std::declval<F>()(std::declval<Args>()...))>, bool> {return 'a';};

        template <typename F, typename ...Args>
        static constexpr short SFINAE (...) {return 0;};
    public:
        template <typename F, typename ...Args>
        static constexpr bool exists = sizeof(SFINAE<F, Args...>(nullptr)) == sizeof(char);
    };

    template <typename F, typename ...Args>
    static constexpr bool does_func_exist = does_func_exist_impl::exists<F, Args...>;

    template <bool b>
    struct convert_type;

    template <>
    struct convert_type<true> : std::true_type {};
    template <>
    struct convert_type<false> : std::false_type {};

    template <typename F, typename ...Args>
    using does_func_exist_type = convert_type<does_func_exist<F, Args...>>;

    template <typename T>
    struct boxed_type {
    private:
        const T& t;

    public:
        boxed_type (const T& _t) : t{_t} {}

        template <typename A, typename B = decltype(std::declval<A>() + t)>
        B operator+ (const A& a) const {
            return t + a;
        };

        template <typename A, typename B = decltype(std::declval<A>() - t)>
        B operator- (const A& a) const {
            return t - a;
        };

        template <typename A, typename B = decltype(std::declval<A>() * t)>
        B operator* (const A& a) const {
            return t * a;
        };

        template <typename A, typename B = decltype(std::declval<A>() / t)>
        B operator/ (const A& a) const {
            return t / a;
        };

        template <typename A, typename B = decltype(std::declval<A>() % t)>
        B operator% (const A& a) const {
            return t % a;
        };

        template <typename A, typename B = decltype(std::declval<A>() ^ t)>
        B operator^ (const A& a) const {
            return t ^ a;
        };

        template <typename A, typename B = decltype(std::declval<A>() & t)>
        B operator& (const A& a) const {
            return t & a;
        };

        template <typename A, typename B = decltype(std::declval<A>() | t)>
        B operator| (const A& a) const {
            return t | a;
        };

        template <typename A, typename B = decltype(std::declval<A>() < t)>
        B operator< (const A& a) const {
            return t < a;
        };

        template <typename A, typename B = decltype(std::declval<A>() > t)>
        B operator > (const A& a) const {
            return t > a;
        };

        template <typename A, typename B = decltype(std::declval<A>() << t)>
        B operator << (const A& a) const {
            return t << a;
        };

        template <typename A, typename B = decltype(std::declval<A>() >> t)>
        B operator >> (const A& a) const {
            return t >> a;
        };

        template <typename A, typename B = decltype(std::declval<A>() == t)>
        B operator== (const A& a) const {
            return t == a;
        }

        template <typename A, typename B = decltype(std::declval<A>() != t)>
        B operator!= (const A& a) const {
            return t != a;
        };

        template <typename A, typename B = decltype(std::declval<A>() <= t)>
        B operator<= (const A& a) const {
            return t <= a;
        };

        template <typename A, typename B = decltype(std::declval<A>() >= t)>
        B operator>= (const A& a) const {
            return t >= a;
        };

        template <typename A, typename B = decltype(std::declval<A>() && t)>
        B operator&& (const A& a) const {
            return t && a;
        };

        template <typename A, typename B = decltype(std::declval<A>() || t)>
        B operator|| (const A& a) const {
            return t || a;
        };
    };

    template <typename T, typename = void>
    struct maybe_boxed_impl;

    template <typename T>
    struct maybe_boxed_impl <T, std::enable_if_t<std::is_fundamental_v<T>>> {
        using val = boxed_type<T>;
    };

    template <typename T>
    struct maybe_boxed_impl <T, std::enable_if_t<!std::is_fundamental_v<T>>> {
        using val = T;
    };

    template <typename T>
    using maybe_boxed = typename maybe_boxed_impl<T>::val;

    template<class... Ts> struct overloaded : Ts... {using Ts::operator()...;
        overloaded(Ts...) {}
    };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}
