#include <exception>
#include <memory>
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
namespace util {
    class InitException : std::runtime_error {
    public:
        explicit InitException (const std::string& error) noexcept :
            std::runtime_error("Initialisation exception: " + error) {};
        const char* getMsg () {
            return what();
        }
    };
}
#endif //EXCEPTIONS_H
