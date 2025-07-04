#pragma once

#include "component_vector.h"
#include "component_view.h"

#include <compare>

namespace phenyl::core {
namespace detail {
    template <typename T>
    class ComponentViewIterator {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        ComponentViewIterator () = default;

        explicit ComponentViewIterator (UntypedComponentVector* vec, std::size_t pos = 0) : m_vec{vec}, m_pos{pos} {}

        value_type& operator* () const {
            return *reinterpret_cast<value_type*>(m_vec->getUntyped(m_pos));
        }

        value_type& operator[] (std::size_t n) const {
            return *reinterpret_cast<value_type*>(m_vec->getUntyped(m_pos + n));
        }

        ComponentViewIterator& operator++ () {
            m_pos++;
            return *this;
        }

        ComponentViewIterator operator++ (int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        ComponentViewIterator& operator-- () {
            m_pos--;
            return *this;
        }

        ComponentViewIterator operator-- (int) {
            auto copy = *this;
            --(*this);
            return copy;
        }

        ComponentViewIterator& operator+= (difference_type n) {
            m_pos += n;
            return *this;
        }

        ComponentViewIterator operator+ (difference_type n) const {
            auto copy = *this;
            copy += n;
            return copy;
        }

        ComponentViewIterator& operator-= (difference_type n) {
            m_pos -= n;
            return *this;
        }

        ComponentViewIterator operator- (difference_type n) const {
            auto copy = *this;
            copy -= n;
            return copy;
        }

        difference_type operator- (const ComponentViewIterator& it) const {
            PHENYL_DASSERT(m_vec == it.m_vec);
            return m_pos - it.m_pos;
        }

        bool operator== (const ComponentViewIterator& other) const {
            return m_vec == other.m_vec && m_pos == other.m_pos;
        }

        std::strong_ordering operator<=> (const ComponentViewIterator& other) const {
            return m_pos <=> other.m_pos;
        }

    private:
        UntypedComponentVector* m_vec = nullptr;
        std::size_t m_pos = 0;
    };

    template <typename T>
    ComponentViewIterator<T> operator+ (typename ComponentViewIterator<T>::difference_type n,
        const ComponentViewIterator<T>& it) {
        return it + n;
    }
} // namespace detail

template <typename T>
class ComponentView {
public:
    using iterator = detail::ComponentViewIterator<T>;
    using const_iterator = detail::ComponentViewIterator<const T>;

    ComponentView (UntypedComponentVector& vec) : m_vec{&vec} {}

    T& operator[] (std::size_t pos) {
        return *reinterpret_cast<T*>(m_vec->getUntyped(pos));
    }

    const T& operator[] (std::size_t pos) const {
        return *reinterpret_cast<T*>(m_vec->getUntyped(pos));
    }

    std::size_t size () const noexcept {
        return m_vec->size();
    }

    template <typename Base>
    requires (std::derived_from<T, Base> && (!std::is_const_v<T> || std::is_const_v<Base>) )
    ComponentView<Base> cast () const {
        return ComponentView<Base>{*m_vec};
    }

    iterator begin () {
        return iterator{m_vec};
    }

    iterator end () {
        return iterator{m_vec, m_vec->size()};
    }

    const_iterator begin () const {
        return cbegin();
    }

    const_iterator cbegin () const {
        return iterator{m_vec};
    }

    const_iterator end () const {
        return cend();
    }

    const_iterator cend () const {
        return iterator{m_vec, m_vec->size()};
    }

private:
    UntypedComponentVector* m_vec;
};
} // namespace phenyl::core
