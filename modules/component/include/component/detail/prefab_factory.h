#pragma once

#include <cstddef>
#include <functional>

namespace phenyl::component::detail {
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
        std::function<T()> factory;
    public:
        explicit FuncPrefabFactory (std::function<T()> factory) : factory{std::move(factory)} {}

        void make (std::byte* ptr) const override {
            T* tPtr = reinterpret_cast<T*>(ptr);

            new (tPtr) T(factory());
        }
    };

    using PrefabFactories = std::map<std::size_t, std::unique_ptr<IPrefabFactory>>;
}