#pragma once

#include <memory>

namespace util {
    namespace detail {
        template <class T>
        struct SmartHelperDefs {
            using Ptr = std::unique_ptr<T>;
            using SharedPtr = std::shared_ptr<T>;
            using WeakPtr = std::weak_ptr<T>;

            template<typename ...Args>
            inline static Ptr NewPtr (Args... args) {
                return std::make_unique<T>(args...);
            }

            template<typename ...Args>
            inline static SharedPtr NewSharedPtr (Args... args) {
                return std::make_shared<T>(args...);
            }
        };
    }
    /*template<class T>
    struct SmartHelper {
        using Ptr = std::unique_ptr<T>;
        using SharedPtr = std::shared_ptr<T>;
        using WeakPtr = std::weak_ptr<T>;

        template<typename ...Args>
        inline static Ptr NewPtr (Args... args) {
            return std::make_unique<T>(args...);
        }

        template<typename ...Args>
        inline static SharedPtr NewSharedPtr (Args... args) {
            return std::make_shared<T>(args...);
        }
    };*/
    template <class T, bool enableShared=false>
    struct SmartHelper;

    template <typename T>
    struct SmartHelper<T, false> : public detail::SmartHelperDefs<T> {};

    template <typename T>
    struct SmartHelper<T, true> : public detail::SmartHelperDefs<T>, public std::enable_shared_from_this<T> {};
}
