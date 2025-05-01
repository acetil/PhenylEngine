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
    public:
        explicit CopyPrefabFactory (T&& obj) : obj{std::move(obj)} {}

        void make (std::byte* ptr) const override {
            T* tPtr = reinterpret_cast<T*>(ptr);

            new (tPtr) T(obj);
        }

    private:
        T obj;
    };

    template <typename T>
    class FuncPrefabFactory : public IPrefabFactory {
    public:
        explicit FuncPrefabFactory (std::function<T()> factory) : m_factory{std::move(factory)} {}

        void make (std::byte* ptr) const override {
            T* tPtr = reinterpret_cast<T*>(ptr);

            new (tPtr) T(m_factory());
        }

    private:
        std::function<T()> m_factory;
    };

    using PrefabFactories = std::map<std::size_t, std::unique_ptr<IPrefabFactory>>;
}