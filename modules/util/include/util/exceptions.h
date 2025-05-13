#pragma once

#include <exception>
#include <memory>

#ifndef NDEBUG
    #include <cpptrace/cpptrace.hpp>
#endif

namespace phenyl {
#ifndef NDEBUG
class PhenylException : public cpptrace::exception_with_message {
public:
    explicit PhenylException (std::string message) : cpptrace::exception_with_message{std::move(message)} {}
};
#else
class PhenylException : public std::runtime_error {
public:
    explicit PhenylException (std::string message) : std::runtime_error{message} {}
};
#endif
} // namespace phenyl
