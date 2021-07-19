#include <type_traits>
#include <functional>
#include <tuple>

#ifndef META_H
#define META_H
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

    template <typename ...Args>
    struct pack_len_impl {
        int packlen = sizeof...(Args);
    };

    template <typename ...Args>
    int pack_len = pack_len_impl<Args...>::packlen;

    template <typename L>
    int typelist_len = type_list_unroll<pack_len_impl, L>::packlen;

    template <int N, typename T, typename L>
    struct is_in_impl;

    template <typename T, typename L>
    struct is_in_impl <0, T, L> {
        bool val = false;
    };

    template <int N, typename T, typename L>
    struct is_in_impl  {
        bool val = std::is_same<get_nth_typelist<N - 1, L>, T>::value || is_in_impl<N - 1, T, L>::val;
    };

    template <typename T, typename L>
    bool is_in_typelist = is_in_impl<typelist_len<L>, T, L>::val;

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

}
#endif
