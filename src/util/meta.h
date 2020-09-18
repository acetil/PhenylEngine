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
}
#endif
