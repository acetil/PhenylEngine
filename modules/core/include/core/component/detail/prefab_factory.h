#pragma once

#include <cstddef>
#include <functional>

namespace phenyl::core::detail {
    class IPrefabFactory {
    public:
        virtual ~IPrefabFactory() = default;

        virtual void make (std::byte* ptr) const = 0;
    };

    template <typename T>
    class CopyPrefabFactory : public IPrefabFactory {
    private:
        T obj;
    public:
        explicit CopyPrefabFactory (T&& obj) : obj{std::move(obj)} {}

        void make (std::byte* ptr) const override {
            T* tPtr = reinterpret_cast<T*>(ptr);

            new (tPtr) T(obj);
        }
    };

    template <typename T>
    class FuncPrefabFactory : public IPrefabFactory {
    private:
        std::function<T()> m_factory;
    public:
        explicit FuncPrefabFactory (std::function<T()> factory) : m_factory{std::move(factory)} {}

        void make (std::byte* ptr) const override {
            T* tPtr = reinterpret_cast<T*>(ptr);

            new (tPtr) T(m_factory());
        }
    };

    using PrefabFactories = std::map<std::size_t, std::unique_ptr<IPrefabFactory>>;
}