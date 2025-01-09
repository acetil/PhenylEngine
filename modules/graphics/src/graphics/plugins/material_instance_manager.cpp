#include "material_instance_manager.h"

#include "core/assets/assets.h"
#include "core/serialization/backends.h"
#include "core/serialization/serializer_impl.h"

using namespace phenyl::graphics;

namespace {
    struct MaterialUniformSerializable : public phenyl::core::ISerializable<std::shared_ptr<MaterialInstance>> {
    public:
        [[nodiscard]] std::string_view name () const noexcept override {
            return "MaterialInstance::uniform";
        }

        void serialize(phenyl::core::ISerializer& serializer, const std::shared_ptr<MaterialInstance>& obj) override {
            PHENYL_ABORT("Material instance serializing is currently unsupported");
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, std::shared_ptr<MaterialInstance>& obj) override {
            static std::string members[] = {"name", "type", "val"};
            deserializer.deserializeStruct(*this, members, obj);
        }

        void deserializeStruct(std::shared_ptr<MaterialInstance>& obj, phenyl::core::IStructDeserializer& deserializer) override {
            if (!deserializer.isNext("name")) {
                throw phenyl::DeserializeException("Failed to deserialize name of material uniform");
            }
            auto name = *deserializer.next<std::string>("name");

            if (!deserializer.isNext("type")) {
                throw phenyl::DeserializeException("Failed to deserialize type of material uniform");
            }
            auto typeName = *deserializer.next<std::string>("type");
            auto type = ShaderTypeFromName(typeName);
            if (type == ShaderDataType::UNKNOWN) {
                throw phenyl::DeserializeException(std::format("Unknown shader data type: \"{}\"", typeName));
            }

            if (!deserializer.isNext("val")) {
                throw phenyl::DeserializeException("Failed to deserialize value of material uniform");
            }

            switch (type) {
                case ShaderDataType::FLOAT32:
                    obj->set(name, *deserializer.next<float>("val"));
                    break;
                case ShaderDataType::INT16:
                    obj->set(name, *deserializer.next<std::int16_t>("val"));
                    break;
                case ShaderDataType::INT32:
                    obj->set(name, *deserializer.next<std::int32_t>("val"));
                    break;
                case ShaderDataType::VEC2F:
                    obj->set(name, *deserializer.next<glm::vec2>("val"));
                    break;
                case ShaderDataType::VEC3F:
                    obj->set(name, *deserializer.next<glm::vec3>("val"));
                    break;
                case ShaderDataType::VEC4F:
                    obj->set(name, *deserializer.next<glm::vec4>("val"));
                    break;
                case ShaderDataType::MAT2F:
                    obj->set(name, *deserializer.next<glm::mat2>("val"));
                    break;
                case ShaderDataType::MAT3F:
                    obj->set(name, *deserializer.next<glm::mat3>("val"));
                    break;
                case ShaderDataType::MAT4F:
                    obj->set(name, *deserializer.next<glm::mat4>("val"));
                    break;
                case ShaderDataType::UNKNOWN:
                    break;
            }
        }
    };

    struct MaterialUniformsSerializable : public phenyl::core::ISerializable<std::shared_ptr<MaterialInstance>> {
    public:
        std::string_view name() const noexcept override {
            return "MaterialInstance::uniforms";
        }
        void serialize(phenyl::core::ISerializer& serializer, const std::shared_ptr<MaterialInstance>& obj) override {
            PHENYL_ABORT("Material instance serializing is currently unsupported");
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, std::shared_ptr<MaterialInstance>& obj) override {
            deserializer.deserializeArray(*this, obj);
        }

        void deserializeArray(std::shared_ptr<MaterialInstance>& obj, phenyl::core::IArrayDeserializer& deserializer) override {
            MaterialUniformSerializable uniformSerializable{};
            while (deserializer.hasNext()) {
                deserializer.next(uniformSerializable, obj);
            }
        }
    };

    struct MaterialInstanceSerializable : public phenyl::core::ISerializable<std::shared_ptr<MaterialInstance>> {
    public:
        std::string_view name () const noexcept override {
            return "MaterialInstance";
        }
        void serialize(phenyl::core::ISerializer& serializer, const std::shared_ptr<MaterialInstance>& obj) override {
            PHENYL_ABORT("Material instance serializing is currently unsupported");
        }

        void deserialize (phenyl::core::IDeserializer& deserializer, std::shared_ptr<MaterialInstance>& obj) override {
            static std::string members[] = {"material", "uniforms"};
            deserializer.deserializeStruct(*this, members, obj);
        }

        void deserializeStruct (std::shared_ptr<MaterialInstance>& obj, phenyl::core::IStructDeserializer& deserializer) override {
            auto matOpt = deserializer.next<phenyl::core::Asset<Material>>("material");
            if (!matOpt) {
                throw phenyl::DeserializeException("Failed to deserialize parent material of instance");
            }

            auto instance = (*matOpt)->instance();

            MaterialUniformsSerializable uniforms;
            if (!deserializer.isNext("uniforms")) {
                throw phenyl::DeserializeException("Failed to deserialize uniforms of material instance");
            }

            deserializer.next("uniforms", uniforms, instance);
            instance->upload();

            obj = std::move(instance);
        }
    };
}

MaterialInstance* MaterialInstanceManager::load (std::ifstream& data, std::size_t id) {
    std::shared_ptr<MaterialInstance> instance;
    MaterialInstanceSerializable serializable{};
    core::DeserializeFromJson(data, serializable, instance);

    auto [it, _] = instances.emplace(id, std::move(instance));
    return it->second.get();
}

MaterialInstance* MaterialInstanceManager::load (MaterialInstance&& obj, std::size_t id) {
    PHENYL_ABORT("Virtual loading of material instances not supported!");
}

const char* MaterialInstanceManager::getFileType () const {
    return ".json";
}

void MaterialInstanceManager::queueUnload (std::size_t id) {
    // TODO
}

void MaterialInstanceManager::selfRegister () {
    core::Assets::AddManager(this);
}

