#pragma once

#include <exception>
#include <memory>

namespace phenyl::util {
    class InitException : std::runtime_error {
    public:
        explicit InitException (const std::string& error) noexcept :
            std::runtime_error("Initialisation exception: " + error) {};
        const char* getMsg () {
            return what();
        }
    };
}
