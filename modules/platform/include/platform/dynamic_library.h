#pragma once

#include "logging/logging.h"

#include <memory>
#include <string>

namespace phenyl::os {
    namespace detail {
        class DynamicLibImpl;
    }

    template <typename R, typename ...Args>
    class DynamicLibFunction {
    private:
        std::weak_ptr<detail::DynamicLibImpl> m_dynamicLibrary;
        R (*func) (Args...);
    public:
        DynamicLibFunction (std::weak_ptr<detail::DynamicLibImpl> dynamicLibrary, void* funcPtr) : m_dynamicLibrary{std::move(dynamicLibrary)}, func{reinterpret_cast<R (*) (Args...)>(funcPtr)} {
            PHENYL_DASSERT(!this->m_dynamicLibrary.expired());
            PHENYL_DASSERT(funcPtr);
        }

        R operator() (Args&&... args) const {
            if (auto guard = m_dynamicLibrary.lock()) {
                return func(std::forward<Args>(args)...);
            } else {
                PHENYL_ABORT("Dynamic library function has been invalidated!");
            }
        }
    };

    class DynamicLibrary {
    private:
        std::shared_ptr<detail::DynamicLibImpl> m_impl;

        void* loadFunctionSymbol (const std::string& symbolName) const;
    public:
        template <typename R, typename ...Args>
        using FunctionType = R (*) (Args...);

        explicit DynamicLibrary (const std::string& path);

        DynamicLibrary (const DynamicLibrary&) = delete;
        DynamicLibrary (DynamicLibrary&&) noexcept;

        DynamicLibrary& operator= (const DynamicLibrary&) = delete;
        DynamicLibrary& operator= (DynamicLibrary&&) noexcept;

        ~DynamicLibrary();

        explicit operator bool () const noexcept {
            return static_cast<bool>(m_impl);
        }

        template <typename R, typename ...Args>
        std::optional<DynamicLibFunction<R, Args...>> function (const std::string& symbolName) const {
            if (auto* func = loadFunctionSymbol(symbolName)) {
                return DynamicLibFunction<R, Args...>{m_impl, func};
            } else {
                return std::nullopt;
            }
        }
    };
}