#include "mock_clock.h"
#include "mock_renderer.h"
#include "phenyl/app_plugin.h"
#include "phenyl/graphics/renderer.h"
#include "platform/dynamic_library.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <phenyl/entrypoint.h>

using namespace phenyl;

static nlohmann::json CollectComponents (PhenylRuntime& runtime);
static nlohmann::json CollectComponent (const core::ComponentInfo& info);

static nlohmann::json CollectTypeInfo (const core::ISerializedTypeInfo& info);
static nlohmann::json CollectPrimitive (const core::PrimitiveSerializedTypeInfo& info);
static nlohmann::json CollectArray (const core::ArraySerializedTypeInfo& info);
static nlohmann::json CollectMap (const core::MapSerializedTypeInfo& info);
static nlohmann::json CollectAsset (const core::AssetSerializedTypeInfo& info);
static nlohmann::json CollectStruct (const core::StructSerializedTypeInfo& info);

int main (int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << std::format("Usage: {} <project_lib>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ApplicationProperties properties;
    properties.withRootLogLevel(LEVEL_FATAL);
    os::DynamicLibrary library{argv[1]};
    if (!library) {
        std::cerr << "Failed to load application library \"" << argv[1] << "\"\n";
        return EXIT_FAILURE;
    }

    auto entrypoint = library.function<void, void*, void*>("phenyl_app_entrypoint");
    if (!entrypoint) {
        std::cerr << "Failed to load application entrypoint\n";
        return EXIT_FAILURE;
    }

    std::unique_ptr<engine::ApplicationBase> app;
    (*entrypoint)(&properties, &app);
    PHENYL_ASSERT(app);

    // Mock engine
    // TODO: make better
    PhenylRuntime runtime{};
    MockClock clock{};
    MockRenderer renderer{};
    runtime.addResource<Renderer>(&renderer);
    runtime.addResource<Clock>(&clock);
    MockShaderManager manager;
    manager.selfRegister();

    // Run app init
    runtime.registerPlugin(std::make_unique<engine::AppPlugin>(std::move(app)));

    nlohmann::json result{{"components", CollectComponents(runtime)}};
    std::cout << result.dump(4) << "\n";
}

nlohmann::json CollectComponents (PhenylRuntime& runtime) {
    std::vector<nlohmann::json> components;
    for (const auto& i : runtime.components()) {
        components.emplace_back(CollectComponent(i));
    }
    return components;
}

nlohmann::json CollectComponent (const core::ComponentInfo& info) {
    nlohmann::json json;
    json["name"] = info.name();
    json["type"] = CollectTypeInfo(info.typeInfo());
    return json;
}

nlohmann::json CollectTypeInfo (const core::ISerializedTypeInfo& info) {
    switch (info.type()) {
    case core::SerializedType::Primitive:
        return CollectPrimitive(info.asPrimitive());
    case core::SerializedType::Array:
        return CollectArray(info.asArray());
    case core::SerializedType::Map:
        return CollectMap(info.asMap());
    case core::SerializedType::Struct:
        return CollectStruct(info.asStruct());
    case core::SerializedType::Asset:
        return CollectAsset(info.asAsset());
    }
    std::cerr << "Unexpected SerializedType: {}" << static_cast<std::uint32_t>(info.type()) << "\n";
    std::exit(1);
}

nlohmann::json CollectPrimitive (const core::PrimitiveSerializedTypeInfo& info) {
    nlohmann::json json;
    json["type"] = "primitive";
    switch (info.primitiveType()) {
    case core::PrimitiveType::Bool:
        json["primitive_type"] = "bool";
        break;
    case core::PrimitiveType::Int8:
        json["primitive_type"] = "int8";
        break;
    case core::PrimitiveType::Int16:
        json["primitive_type"] = "int16";
        break;
    case core::PrimitiveType::Int32:
        json["primitive_type"] = "int32";
        break;
    case core::PrimitiveType::Int64:
        json["primitive_type"] = "int64";
        break;
    case core::PrimitiveType::Uint8:
        json["primitive_type"] = "uint8";
        break;
    case core::PrimitiveType::Uint16:
        json["primitive_type"] = "uint16";
        break;
    case core::PrimitiveType::Uint32:
        json["primitive_type"] = "uint32";
        break;
    case core::PrimitiveType::Uint64:
        json["primitive_type"] = "uint64";
        break;
    case core::PrimitiveType::Float:
        json["primitive_type"] = "float";
        break;
    case core::PrimitiveType::Double:
        json["primitive_type"] = "double";
        break;
    case core::PrimitiveType::String:
        json["primitive_type"] = "string";
        break;
    case core::PrimitiveType::Vec2:
        json["primitive_type"] = "vec2";
        break;
    case core::PrimitiveType::Vec3:
        json["primitive_type"] = "vec3";
        break;
    case core::PrimitiveType::Vec4:
        json["primitive_type"] = "vec4";
        break;
    case core::PrimitiveType::Mat2:
        json["primitive_type"] = "mat2";
        break;
    case core::PrimitiveType::Mat3:
        json["primitive_type"] = "mat3";
        break;
    case core::PrimitiveType::Mat4:
        json["primitive_type"] = "mat4";
        break;
    case core::PrimitiveType::Quaternion:
        json["primitive_type"] = "quaternion";
        break;
    default:
        std::cerr << "Unexpected PrimitiveType: " << static_cast<std::uint32_t>(info.primitiveType()) << "\n";
        std::exit(1);
    }
    return json;
}

nlohmann::json CollectArray (const core::ArraySerializedTypeInfo& info) {
    nlohmann::json json;
    json["type"] = "array";
    json["element_type"] = CollectTypeInfo(info.elementType());
    return json;
}

nlohmann::json CollectMap (const core::MapSerializedTypeInfo& info) {
    nlohmann::json json;
    json["type"] = "map";
    json["value_type"] = CollectTypeInfo(info.valueType());
    return json;
}

nlohmann::json CollectAsset (const core::AssetSerializedTypeInfo& info) {
    nlohmann::json json;
    json["type"] = "asset";
    json["asset_type"] = info.asset();
    return json;
}

nlohmann::json CollectStruct (const core::StructSerializedTypeInfo& info) {
    nlohmann::json json;
    json["type"] = "struct";
    std::vector<nlohmann::json> members;
    for (const auto& [memName, i] : info.members()) {
        nlohmann::json memberJson;
        memberJson["name"] = memName;
        memberJson["type"] = CollectTypeInfo(*i);
        members.emplace_back(std::move(memberJson));
    }
    json["members"] = std::move(members);
    return json;
}
