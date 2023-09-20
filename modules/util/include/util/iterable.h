#pragma once

namespace phenyl::util {
    template <typename T>
    class Iterable {
    private:
        T beginIt;
        T endIt;
    public:
        using iterator = T;

        Iterable (T beginIt, T endIt) : beginIt{beginIt}, endIt{endIt} {}

        T begin () const {
            return beginIt;
        }

        T end () const {
            return endIt;
        }
    };
}