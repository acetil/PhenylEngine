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
        std::aligned_storage_t<sizeof(T), alignof(T)> data;
        bool initialised = false;
    public:
        Memory () = default;

        template <typename ...Args>
        explicit Memory (Args&&... args) {
            ::new(&data) T(std::forward<Args&&>(args)...);
            initialised = true;
        }

        explicit Memory (T& val) {
            ::new(&data) T(val);
            initialised = true;
        }

        explicit Memory (T&& val) noexcept {
            ::new(&data) T(std::move(val));
            initialised = true;
        }


        Memory (Memory& other) {
            ::new(&data) T(other.data);
            initialised = true;
        }

        Memory (Memory&& other)  noexcept {
            ::new(&data) T(std::move(other.mget()));
            initialised = true;
            other.initialised = false;
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
            return *const_cast<T*>(reinterpret_cast<const T*>(&data));
        }

        const T& get () const {
            return *reinterpret_cast<const T*>(&data);
        }

        void clear () {
            if (initialised) {
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
        std::aligned_storage_t<sizeof(T*), alignof(T*)> data;
    public:
        RefMemory() = default;
        explicit RefMemory(T& val) {
            *reinterpret_cast<T**>(&data) = (reinterpret_cast<T*>(&val));
        }

        //RefMemory<T>& operator= (RefMemory<T>&) = delete;
        //RefMemory<T>& operator= (RefMemory<T>&&) = delete;

        T& get () const {
            return **reinterpret_cast<T* const*>(&data);
        }
    };
}
#endif
