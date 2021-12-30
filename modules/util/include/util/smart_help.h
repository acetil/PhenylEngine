#include <memory>

#ifndef SMART_HELP_H
#define SMART_HELP_H
namespace util {
    template<class T>
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
    };
}
#endif
