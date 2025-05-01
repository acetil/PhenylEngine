#include <type_traits>
#include <utility>

#ifndef DETAIL_MEMORY_H
#define DETAIL_MEMORY_H
namespace phenyl::util::detail {

    template <bool b>
    class Copyable;

    template<>
    class Copyable<true> {
    protected:
        ~Copyable() = default;
    };

    template<>
    class Copyable<false> {
    public:
        Copyable() = default;
        Copyable(Copyable&) = delete;
        Copyable& operator= (Copyable&) = delete;
    protected:
        ~Copyable() = default;
    };

    template <bool b>
    class Movable;

    template<>
    class Movable<true> {
    protected:
        ~Movable() = default;
    };

    template<>
    class Movable<false> {
    public:
        Movable() = default;
        Movable(Movable&&) = delete;
        Movable& operator= (Movable&&) = delete;
    protected:
        ~Movable() = default;
    };

    // TODO: improve copy/move assignment
    template <typename T>
    class Memory : public Copyable<std::is_copy_assignable_v<T> || std::is_copy_constructible_v<T>> {
    private:
        alignas(T) std::byte m_data[sizeof(T)];
        bool m_initialized = false;
    public:
        Memory () = default;

        template <typename ...Args>
        explicit Memory (Args&&... args) {
            ::new(&m_data) T(std::forward<Args&&>(args)...);
            m_initialized = true;
        }

        explicit Memory (T& val) {
            ::new(&m_data) T(val);
            m_initialized = true;
        }

        explicit Memory (T&& val) noexcept {
            ::new(&m_data) T(std::move(val));
            m_initialized = true;
        }


        Memory (Memory& other) {
            ::new(&m_data) T(other.data);
            m_initialized = true;
        }

        Memory (Memory&& other)  noexcept {
            ::new(&m_data) T(std::move(other.mget()));
            m_initialized = true;
            other.m_initialized = false;
        }

        Memory<T>& operator= (Memory const& other) {
            mget() = other.get();
            return *this;
        }

        Memory<T>& operator= (Memory&& other)  noexcept {
            mget() = std::move(other.mget());
            return *this;
        }

        T& mget () const {
            return *const_cast<T*>(reinterpret_cast<const T*>(&m_data));
        }

        const T& get () const {
            return *reinterpret_cast<const T*>(&m_data);
        }

        void clear () {
            if (m_initialized) {
                get().~T();
            }
        }

        ~Memory() {
            clear();
        }
    };

    template <typename T>
    class RefMemory {
    private:
        T* m_data;
    public:
        RefMemory() = default;
        explicit RefMemory(T& val) {
            m_data = &val;
        }

        //RefMemory<T>& operator= (RefMemory<T>&) = delete;
        //RefMemory<T>& operator= (RefMemory<T>&&) = delete;

        T& get () const {
            return *m_data;
        }
    };
}
#endif
