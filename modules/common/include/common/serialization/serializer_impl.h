#pragma once

#include <functional>
#include <unordered_map>
#include <utility>

#include "common/detail/loggers.h"
#include "graphics/maths_headers.h"
#include "serializer.h"

namespace phenyl::common::detail {
    template <SerializableType T>
    class VectorSerializable : public ISerializable<std::vector<T>> {
    public:
        [[nodiscard]] std::string_view name () const noexcept override {
            return std::format("std::vector<{}>", GetSerializable<T>().name());
        }

        void serialize (ISerializer& serializer, const std::vector<T>& obj) override {
            IArraySerializer& arrSerializer = serializer.serializeArr();
            for (const auto& i : obj) {
                arrSerializer.serializeElement(i);
            }
            arrSerializer.end();
        }

        void deserialize (IDeserializer& deserializer, std::vector<T>& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (std::vector<T>& obj, IArrayDeserializer& deserializer) override {
            if (auto sizeOpt = deserializer.tryGetSize(); sizeOpt) {
                obj.reserve(*sizeOpt);
            }

            while (deserializer.hasNext()) {
                obj.emplace_back(deserializer.next<T>());
            }
        }
    };
    template <SerializableType T>
    ISerializable<std::vector<T>>& phenyl_GetSerializable(SerializableMarker<std::vector<T>>) {
        static ISerializable<std::vector<T>> serializable{};
        return serializable;
    }

    template <SerializableType T>
    class MapSerializable : public ISerializable<std::unordered_map<std::string, T>> {
    public:
        std::string_view name () const noexcept override {
            return std::format("std::unordered_map<std::string, {}>", GetSerializable<T>().name());
        }

        void serialize (ISerializer& serializer, const std::unordered_map<std::string, T>& obj) override {
            IObjectSerializer& objSerializer = serializer.serializeObj();
            for (const auto& [i, val] : obj) {
                objSerializer.serializeMember(i, val);
            }
            objSerializer.end();
        }

        void deserialize (IDeserializer& deserializer, std::unordered_map<std::string, T>& obj) override {
            deserializer.deserializeObject(*this, obj);
        }

        void deserializeObject (std::unordered_map<std::string, T>& obj, IObjectDeserializer& deserializer) override {
            while (deserializer.hasNext()) {
                auto key = deserializer.nextKey();
                obj.emplace(std::string{key, deserializer.nextValue<T>()});
            }
        }
    };

    template <SerializableType T>
    ISerializable<std::unordered_map<std::string, T>>& phenyl_GetSerializable(SerializableMarker<std::unordered_map<std::string, T>>) {
        static ISerializable<std::unordered_map<std::string, T>> serializable{};
        return serializable;
    }

     template <SerializableType T, glm::length_t N>
    class VecSerializable : public ISerializable<glm::vec<N, T>> {
    private:
        std::string vecName;
    public:
        using Vec = glm::vec<N, T>;
        explicit VecSerializable (std::string vecName) : vecName{std::move(vecName)} {}

        std::string_view name () const noexcept override {
            return vecName;
        }

        void serialize (ISerializer& serializer, const Vec& obj) {
            auto& arrSerializer = serializer.serializeArr();

            for (auto i = 0; i < N; i++) {
                arrSerializer.serializeElement(obj[i]);
            }
            arrSerializer.end();
        }

        void deserialize (IDeserializer& deserializer, Vec& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (Vec& obj, IArrayDeserializer& deserializer) override {
            auto sizeOpt = deserializer.tryGetSize();
            if (sizeOpt && *sizeOpt != N) {
                throw phenyl::DeserializeException(std::format("Invalid length array for {}: expected {}, got {}", name(), N, *sizeOpt));
            }

            int i = 0;
            while (deserializer.hasNext()) {
                if (i >= N) {
                    throw phenyl::DeserializeException(std::format("Invalid length array for {}: expected {}", name(), N));
                }

                obj[i] = deserializer.next<float>();
                i++;
            }
        }
    };

    template <SerializableType T, glm::length_t H, glm::length_t W>
    class MatSerializable : public ISerializable<glm::mat<H, W, T>> {
    private:
        std::string matName;
    public:
        using Mat = glm::mat<H, W, T>;
        MatSerializable (std::string matName) : matName{std::move(matName)} {}

        std::string_view name () const noexcept override {
            return matName;
        }

        void serialize (ISerializer& serializer, const Mat& obj) {
            auto& arrSerializer = serializer.serializeArr();

            for (auto i = 0; i < H; i++) {
                arrSerializer.serializeElement(obj[i]);
            }
            arrSerializer.end();
        }

        void deserialize (IDeserializer& deserializer, Mat& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray (Mat& obj, IArrayDeserializer& deserializer) override {
            auto sizeOpt = deserializer.tryGetSize();
            if (sizeOpt && *sizeOpt != H) {
                throw phenyl::DeserializeException(std::format("Invalid length array for {}: expected {}, got {}", name(), H, *sizeOpt));
            }

            int i = 0;
            while (deserializer.hasNext()) {
                if (i >= H) {
                    throw phenyl::DeserializeException(std::format("Invalid length array for {}: expected {}", name(), H));
                }

                obj[i] = deserializer.next<glm::vec<W, T>>();
                i++;
            }
        }
    };

    template <glm::length_t L>
    ISerializable<glm::vec<L, float>>& phenyl_GetSerializable (SerializableMarker<glm::vec<L, float>>) {
        static VecSerializable<float, L> serializable{std::format("glm::vec{}", L)};
        return serializable;
    }

    template <glm::length_t H, glm::length_t W>
    ISerializable<glm::mat<H, W, float>>& phenyl_GetSerializable (SerializableMarker<glm::mat<H, W, float>>) {
        static MatSerializable<float, H, W> serializable{H == W ? std::format("glm::mat{}", H) : std::format("glm::mat{}x{}", H, W)};
        return serializable;
    }

    template <typename T>
    class IMemberSerializable {
    public:
        virtual ~IMemberSerializable () = default;

        virtual std::string_view getKey () const noexcept = 0;

        virtual void serialize (IObjectSerializer& serializer, const T* obj) = 0;
        virtual void deserialize (IObjectDeserializer& deserializer, T* obj) = 0;
        virtual bool deserialize (IStructDeserializer& deserializer, T* obj) = 0;
    };

    template <typename T, SerializableType M>
    class MemberSerializable : public IMemberSerializable<T> {
    private:
        std::string_view name;
        M T::*member;
    public:
        MemberSerializable (std::string_view name, M T::*member) : name{name}, member{member} {}

        std::string_view getKey () const noexcept override {
            return name;
        }

        void serialize (IObjectSerializer& serializer, const T* obj) override {
            serializer.serializeMember(name, obj->*member);
        }

        void deserialize (IObjectDeserializer& deserializer, T* obj) override {
            obj->*member = deserializer.nextValue<M>();
        }

        bool deserialize (IStructDeserializer& deserializer, T* obj) override {
            return deserializer.next(name, obj->*member);
        }
    };

    template <typename T, typename M, typename F1, typename F2>
    class MethodSerializable : public IMemberSerializable<T> {
    private:
        std::string_view name;
        F1 getter;
        F2 setter;
    public:
        MethodSerializable (std::string_view name, F1&& getter, F2&& setter) : name{name}, getter{std::move(getter)}, setter{std::move(setter)} {}

        std::string_view getKey () const noexcept override {
            return name;
        }

        void serialize (IObjectSerializer& serializer, const T* obj) override {
            serializer.serializeMember(name, std::invoke(getter, obj));
        }

        void deserialize (IObjectDeserializer& deserializer, T* obj) override {
            std::invoke(setter, obj, deserializer.nextValue<M>());
        }

        bool deserialize (IStructDeserializer& deserializer, T* obj) override {
            std::optional<M> valOpt = deserializer.next<M>(name);
            if (valOpt) {
                std::invoke(setter, obj, std::move(*valOpt));
                return true;
            } else {
                return false;
            }
        }
    };

    template <typename T, SerializableType B> requires std::derived_from<T, B>
    class InheritsSerializable : public IMemberSerializable<T> {
    private:
        std::string_view name;
    public:
        explicit InheritsSerializable (std::string_view name) : name{name} {}

        std::string_view getKey () const noexcept override {
            return name;
        }

        void serialize (IObjectSerializer& serializer, const T* obj) override {
            serializer.serializeMember(name, *static_cast<const B*>(obj));
        }

        void deserialize (IObjectDeserializer& deserializer, T* obj) override {
            deserializer.nextValue(*static_cast<B*>(obj));
        }

        bool deserialize (IStructDeserializer& deserializer, T* obj) override {
            return deserializer.next(name, *static_cast<B*>(obj));
        }
    };

    template <typename T>
    class ClassSerializable : public ISerializable<T> {
    private:
        std::string_view className;
        //std::unordered_map<std::string_view, std::unique_ptr<IMemberSerializable<T>>> members;
        std::vector<std::unique_ptr<IMemberSerializable<T>>> members;
        std::vector<std::string> memberNames;

        template <typename ...Args>
        void addAll (std::unique_ptr<Args>... args) {
            members.reserve(sizeof...(Args));
            ([&] {
                members.emplace_back(std::move(args));
            } (), ...);
        }
    public:
        template <typename ...Args>
        explicit ClassSerializable (std::string_view className, std::unique_ptr<Args>... args) : className{className} {
            addAll(std::move(args)...);
            for (auto& member : members) {
                memberNames.emplace_back(std::string{member->getKey()});
            }
        }

        [[nodiscard]] std::string_view name () const noexcept override {
            return className;
        }

        void serialize (ISerializer& serializer, const T& obj) override {
            IObjectSerializer& objSerializer = serializer.serializeObj();

            /*for (auto& [_, member] : members) {
                member->serialize(objSerializer, &obj);
            }*/
            for (auto& member : members) {
                member->serialize(objSerializer, &obj);
            }
            objSerializer.end();
        }

        void deserialize (IDeserializer& deserializer, T& obj) override {
            //deserializer.deserializeObject(*this, obj);
            deserializer.deserializeStruct(*this, std::span{memberNames}, obj);
        }

        /*void deserializeObject (T& obj, IObjectDeserializer& deserializer) override {
            std::size_t deserializedMembers = 0;
            while (deserializer.hasNext()) {
                auto key = deserializer.nextKey();
                auto it = members.find(key);
                if (it != members.end()) {
                    it->second->deserialize(deserializer, &obj);
                    deserializedMembers++;
                } else {
                    PHENYL_LOGE(SERIALIZER_LOGGER, "Ignoring member \"{}\" in class {}", key, name());
                    deserializer.ignoreNextValue();
                }
            }

            if (deserializedMembers != members.size()) {
                throw DeserializeException(std::format("Failed to deserialize all members of class {}", className));
            }
        }*/

        void deserializeStruct (T& obj, IStructDeserializer& deserializer) override {
            for (auto& member : members) {
                if (!member->deserialize(deserializer, &obj)) {
                    throw DeserializeException(std::format("Failed to deserialize member \"{}\" of class {}", member->getKey(), name()));
                }
            }
        }
    };
}

#define PHENYL_SERIALIZABLE_FUNCTION(T, inlineSpecifier, ...) inlineSpecifier ::phenyl::common::ISerializable<T>& phenyl_GetSerializable (::phenyl::common::detail::SerializableMarker<T>) { \
    using Type = T;\
    static ::phenyl::common::detail::ClassSerializable<Type> serializable{#T __VA_OPT__(,) __VA_ARGS__}; \
    return serializable; \
}\

#define PHENYL_SERIALIZABLE(T, ...) PHENYL_SERIALIZABLE_FUNCTION(T, __VA_OPT__(,) __VA_ARGS__)
#define PHENYL_SERIALIZABLE_INLINE(T, ...) PHENYL_SERIALIZABLE_FUNCTION(T, inline, __VA_OPT__(,) __VA_ARGS__)

#define PHENYL_SERIALIZABLE_MEMBER_NAMED(member, memberName) std::make_unique<::phenyl::common::detail::MemberSerializable<Type, std::remove_cvref_t<decltype(std::declval<Type>().member)>>>(memberName, &Type::member)
#define PHENYL_SERIALIZABLE_MEMBER(member) PHENYL_SERIALIZABLE_MEMBER_NAMED(member, #member)

#define PHENYL_SERIALIZABLE_METHOD(memberName, getter, setter) std::make_unique<::phenyl::common::detail::MethodSerializable<Type, std::remove_cvref_t<std::result_of_t<decltype(getter)(const Type*)>>, std::remove_cvref_t<decltype(getter)>, std::remove_cvref_t<decltype(setter)>>>(memberName, getter, setter)

#define PHENYL_SERIALIZABLE_INHERITS_NAMED(Base, baseName) std::make_unique<::phenyl::common::detail::InheritsSerializable<Type, Base>>(baseName)
#define PHENYL_SERIALIZABLE_INHERITS(Base) PHENYL_SERIALIZABLE_INHERITS_NAMED(Base, #Base)