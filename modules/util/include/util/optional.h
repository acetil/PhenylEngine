#pragma once

#include <type_traits>
#include <exception>

#ifndef NDEBUG
// Fix for clangd incorrectly warning about source_location
#ifndef PHENYL_CLANGD
#include <source_location>
#else
#include <experimental/source_location>
namespace std {
    using source_location = std::experimental::source_location;
}
#endif
#include "logging/logging.h"
#endif

#include "detail/memory.h"
//#include "meta.h"

namespace util {
#ifndef PHENYL_OPTIONAL_DECLARE
#define PHENYL_OPTIONAL_DECLARE
    template <typename T, typename = void>
    class Optional;
#endif
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

    // TODO: improve copy/move constructor/assignment
    template <typename T>
    class Optional<T, std::enable_if_t<!std::is_reference_v<T>>> {
    private:
        bool hasVal;
        detail::Memory<T> memory;
    public:
        Optional () : memory{}, hasVal{false} {}
        Optional (detail::NullOpt_t) : memory{}, hasVal{false} {}

        Optional (const Optional<T>& other) : hasVal{other.hasVal}, memory{} {
            if (hasVal) {
                memory = detail::Memory<T>{other.memory.get()};
            }
        }
        Optional (Optional<T>&& other) noexcept : hasVal{other.hasVal}, memory{std::move(other.memory)} {
            other.memory.clear();
            other.hasVal = false;
        }

        Optional& operator= (const Optional<T>& other) {
            if (hasVal && !other.hasVal) {
                memory.clear();
            } else if (!hasVal && other.hasVal) {
                memory = detail::Memory<T>(other.memory.get());
            } else {
                memory.mget() = other.memory.get();
            }

            hasVal = other.hasVal;

            return *this;
        }

        Optional& operator= (Optional<T>&& other) noexcept {
            if (hasVal && !other.hasVal) {
                memory.clear();
            } else if (!hasVal && other.hasVal) {
                memory = detail::Memory<T>(std::move(other.memory.mget()));
            } else {
                memory.mget() = std::move(other.memory.mget());
            }

            hasVal = other.hasVal;
            other.hasVal = false;
            other.memory.clear();

            return *this;
        }

        Optional (T& val) : memory{val}, hasVal(true) {}
        Optional (T&& val) : memory{std::forward<T&&>(val)}, hasVal(true) {}

        template <typename ...Args>
        Optional (Args&&... args) : memory{args...}, hasVal{true} {}

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
        T& getUnsafe (const std::source_location& loc = std::source_location::current()) const {
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
        Optional<T> orOpt (F f) const noexcept {
            return hasVal ? *this : f();
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

        template <typename F>
        Optional<T>& thenIfPresent (F f) noexcept {
            if (hasVal) {
                f(memory.mget());
            }
            return *this;
        }

        template <typename F>
        const Optional<T>& thenIfPresent (F f) const noexcept {
            if (hasVal) {
                f(memory.mget());
            }
            return *this;
        }

        template <typename F>
        void ifNotPresent (F f) const noexcept {
            if (!hasVal) {
                f();
            }
        }

        template <typename F>
        Optional<T>& thenIfNotPresent (F f) const noexcept {
            if (!hasVal) {
                f();
            }
            return *this;
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
        Optional<T&> orOpt (F f) const noexcept {
            return hasVal ? *this : f();
        }

        template <typename F>
        auto then (F f) const noexcept -> decltype(f(memory.get())){
            return hasVal ? f(memory.get()) : NullOpt;
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

        template <typename F>
        const Optional<T&>& thenIfPresent (F f) const noexcept {
            if (hasVal) {
                f(memory.get());
            }
            return *this;
        }

        template <typename F>
        void ifNotPresent (F f) const noexcept {
            if (!hasVal) {
                f();
            }
        }

        template <typename F>
        const Optional<T&>& thenIfNotPresent (F f) const noexcept {
            if (!hasVal) {
                f();
            }
            return *this;
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
    concept IsNotReference = !std::is_reference_v<T>;

    template <IsNotReference T>
    Optional (T&&) -> util::Optional<T>;

    template <IsNotReference T>
    Optional (const T&) -> util::Optional<T>;
}
