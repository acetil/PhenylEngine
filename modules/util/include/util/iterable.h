#pragma once

namespace phenyl::util {
    template <typename T>
    class Iterable {
    public:
        using iterator = T;

        Iterable (T beginIt, T endIt) : m_begin{beginIt}, m_end{endIt} {}

        T begin () const {
            return m_begin;
        }

        T end () const {
            return m_end;
        }

    private:
        T m_begin;
        T m_end;
    };
}