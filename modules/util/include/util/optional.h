#pragma once

#include <type_traits>
#include <exception>

#ifndef NDEBUG
#include <source_location>
#endif

#include "detail/memory.h"
//#include "meta.h"

namespace util {
    template <typename T, typename = void>
    class Optional;

    namespace detail {
        class NullOpt_t {
            int val;
        public:
            constexpr NullOpt_t () : val(0) {}
        };

        template <typename T>
        struct remove_optional_impl {
            static constexpr bool val = false;
        };
        template <typename T>
        struct remove_optional_impl<util::Optional<T>> {
            static constexpr bool val = true;
            using type = T;
        };

        template <typename T>
        using remove_optional = typename remove_optional_impl<T>::type;
    }

    constexpr detail::NullOpt_t NullOpt;

    template <typename T>
    class Optional<T, std::enable_if_t<!std::is_reference_v<T>>> {
    private:
        bool hasVal;
        detail::Memory<T> memory;
    public:
        Optional () : memory{}, hasVal{false} {}
        Optional (detail::NullOpt_t) : memory{}, hasVal{false} {}

        explicit Optional (T& val) : memory{val}, hasVal(true) {}
        explicit Optional (T&& val) : memory{std::forward<T&&>(val)}, hasVal(true) {}

        template <typename ...Args>
        explicit Optional (Args&&... args) : memory{args...}, hasVal{true} {}

        const T& orElse (const T& otherVal) const noexcept {
            return hasVal ? memory.get() : otherVal;
        }

        T& orThrow () const {
            if (!hasVal) {
                throw std::runtime_error("Bad exception access!");
            }
            return memory.mget();
        }

#ifdef NDEBUG
        const T& getUnsafe () const {
            return memory.get();
        }
#else
        T& getUnsafe (const std::source_location loc = std::source_location::current()) const {
            if (!hasVal) {
                logging::log(LEVEL_FATAL, "Unsafe optional get of empty opt at {}({}:{}) ({})!", loc.file_name(), loc.line(), loc.column(), loc.function_name());
            }
            return memory.mget();
        }
#endif

        template <typename F>
        const T& orElseGet (F f) const noexcept {
            return hasVal ? memory.get() : f();
        }

        template <typename F>
        auto then (F f) const noexcept -> decltype(f(memory.get())){
            return hasVal ? f(memory.get()) : NullOpt;
        }

        template <typename F>
        auto thenMap (F f) const noexcept -> Optional<decltype(f(memory.get()))> {
            /*if (hasVal) {
                return Optional<decltype(f(memory.get()))>(f(memory.get()));
            }

            return NullOpt;*/
            return hasVal ? Optional<decltype(f(memory.get()))>(f(memory.get())) : NullOpt;
        }

        template <typename F>
        void ifPresent (F f) noexcept {
            if (hasVal) {
                f(memory.mget());
            }
        }

        template <typename Dummy = T>
        auto flatMap () -> const Optional<detail::remove_optional<Dummy>>& {
            return hasVal ? memory.get() : NullOpt;
        }

        bool isEmpty () {
            return !hasVal;
        }

        operator bool () {
            return hasVal;
        }
    };

    template <typename T>
    class Optional<T&> {
    private:
        bool hasVal;
        detail::RefMemory<T> memory;
    public:
        Optional () : memory{}, hasVal{false} {}
        Optional (detail::NullOpt_t) : memory{}, hasVal{false} {}

        explicit Optional (T& val) : memory{val}, hasVal{true} {}
        explicit Optional (T&& val) : memory{val}, hasVal(true) {}

        const T& orElse (const T& otherVal) const noexcept {
            return hasVal ? memory.get() : otherVal;
        }

        T& orThrow () const {
            if (!hasVal) {
                throw std::runtime_error("Bad exception access!");
            }
            return memory.mget();
        }

#ifdef NDEBUG
        const T& getUnsafe () const {
            return memory.get();
        }
#else
        T& getUnsafe (const std::source_location loc = std::source_location::current()) const {
            if (!hasVal) {
                logging::log(LEVEL_FATAL, "Unsafe optional get at {}({}:{}) ({})!", loc.file_name(), loc.line(), loc.column(), loc.function_name());
            }
            return memory.get();
        }
#endif
        template <typename F>
        const T& orElseGet (F f) const noexcept {
            return hasVal ? memory.get() : f();
        }

        template <typename F>
        auto then (F f) const noexcept -> decltype(f(memory.get())){
            return hasVal ? f(memory.get()) : Optional();
        }

        template <typename F>
        auto thenMap (F f) const noexcept -> Optional<decltype(f(memory.get()))> {
            return hasVal ? Optional<decltype(f(memory.get()))>{f(memory.get())} : NullOpt;
        }

        template <typename F>
        void ifPresent (F f) const noexcept {
            if (hasVal) {
                f(memory.get());
            }
        }

        template <typename Dummy = T>
        auto flatMap () -> const Optional<detail::remove_optional<Dummy>>& {
            return hasVal ? memory.get() : NullOpt;
        }

        bool isEmpty () {
            return !hasVal;
        }

        operator bool () {
            return hasVal;
        }
    };

}
