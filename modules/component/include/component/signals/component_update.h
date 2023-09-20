#pragma once

#include <cstddef>
#include <concepts>

namespace phenyl::component {
    namespace detail {
        struct OnInsertUntyped;
        struct OnStatusChangeUntyped;
        struct OnRemoveUntyped;
    }

    template <typename T>
    struct OnInsert {
    private:
        T* comp;
        OnInsert (T* comp) : comp{comp} {}
        friend detail::OnInsertUntyped;
    public:
        static constexpr bool PreOrder = true;
        using Type = T;

        T& get () const {
            return *comp;
        }
    };

    template <typename T>
    struct OnStatusChange {
    private:
        T* comp;
        bool newStatus;

        OnStatusChange (T* comp, bool status) : comp{comp}, newStatus{status} {}
        friend detail::OnStatusChangeUntyped;
    public:
        static constexpr bool PreOrder = true;
        using Type = T;

        T& get () const {
            return *comp;
        }

        bool active () const {
            return newStatus;
        }

        bool inactive () const {
            return !newStatus;
        }
    };

    template <typename T>
    struct OnRemove {
    private:
        T* comp;

        OnRemove (T* comp) : comp {comp} {}
        friend detail::OnRemoveUntyped;
    public:
        static constexpr bool PreOrder = false;
        using Type = T;

        const T& get () const {
            return *comp;
        }
    };

    namespace detail {
        template <typename T>
        struct IsOnInsert {
            static constexpr bool val = false;
        };

        template <typename T>
        struct IsOnInsert<OnInsert<T>> {
            static constexpr bool val = true;
        };

        template <typename T>
        struct IsOnStatusChange {
            static constexpr bool val = false;
        };

        template <typename T>
        struct IsOnStatusChange<OnStatusChange<T>> {
            static constexpr bool val = true;
        };

        template <typename T>
        struct IsOnRemove {
            static constexpr bool val = false;
        };

        template <typename T>
        struct IsOnRemove<OnRemove<T>> {
            static constexpr bool val = true;
        };

        template <typename T>
        static constexpr bool IsComponentSignal = (IsOnInsert<T>::val || IsOnStatusChange<T>::val || IsOnRemove<T>::val);

        struct OnInsertUntyped {
            std::byte* comp;

            static constexpr bool PreOrder = true;

            template <typename T>
            using Signal = OnInsert<T>;

            template <typename T>
            Signal<T> convert () const {
                return Signal<T>{(T*)comp};
            }
        };

        struct OnStatusChangeUntyped {
            std::byte* comp;
            bool status;

            static constexpr bool PreOrder = true;

            template <typename T>
            using Signal = OnStatusChange<T>;

            template <typename T>
            Signal<T> convert () const {
                return Signal<T>{(T*)comp, status};
            }
        };

        struct OnRemoveUntyped {
            std::byte* comp;

            static constexpr bool PreOrder = false;

            template <typename T>
            using Signal = OnRemove<T>;

            template <typename T>
            Signal<T> convert () const {
                return Signal<T>{(T*)comp};
            }
        };

        template <typename T>
        concept UntypedComponentSignal = (std::same_as<T, OnInsertUntyped> || std::same_as<T, OnStatusChangeUntyped> || std::same_as<T, OnRemoveUntyped>) && requires {
            { T::PreOrder } -> std::convertible_to<bool>;
        };

        template <typename T, typename U>
        concept TypedUntypedComponentSignal = UntypedComponentSignal<T> && requires (T t) {
            typename T::template Signal<U>;
            { t.template convert<U>() } -> std::convertible_to<typename T::template Signal<U>>;
        };
    }

    template <typename T>
    concept ComponentSignal = requires {
        requires detail::IsComponentSignal<T>;
        typename T::Type;
    };

    template <typename T, typename U>
    concept TypedComponentSignal = std::same_as<T, OnInsert<U>> || std::same_as<T, OnStatusChange<U>> || std::same_as<T, OnRemove<U>>;
}