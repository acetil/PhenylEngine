#include "material_manager.h"

#include "core/assets/assets.h"
#include "core/serialization/backends.h"
#include "core/serialization/serializer_impl.h"

#include <fstream>

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"MATERIAL_MANAGER", detail::GRAPHICS_LOGGER};

namespace {
struct UniformDefinition {
    std::string name;
    std::string type;
};

PHENYL_SERIALIZABLE(UniformDefinition, PHENYL_SERIALIZABLE_MEMBER(name), PHENYL_SERIALIZABLE_MEMBER(type))

struct MaterialDefinition {
    std::shared_ptr<Shader> shader;
    std::vector<UniformDefinition> uniforms;
};

PHENYL_SERIALIZABLE(MaterialDefinition, PHENYL_SERIALIZABLE_MEMBER(shader), PHENYL_SERIALIZABLE_MEMBER(uniforms))
} // namespace

MaterialManager::MaterialManager (Renderer& renderer) : m_renderer{renderer} {}

std::shared_ptr<Material> MaterialManager::load (core::AssetLoadContext& ctx) {
    return ctx.withExtension(".json").deserialize([&] (MaterialDefinition&& def) -> std::shared_ptr<Material> {
        MaterialProperties props;
        for (auto& i : def.uniforms) {
            auto type = ShaderTypeFromName(i.type);
            if (type == ShaderDataType::UNKNOWN) {
                PHENYL_LOGE(LOGGER, "Failed to parse uniform type \"{}\" for property \"{}\"", i.type, i.name);
                continue;
            }

            if (auto off = def.shader->uniformOffset("Material", i.name); off) {
                props.uniforms.emplace_back(i.name, type, *off);
            } else {
                PHENYL_LOGE(LOGGER, "Failed to find uniform \"{}\" in shader", i.name);
            }
        }

        props.uniformBlockSize = *def.shader->uniformBlockSize("Material");

        // TODO
        return std::make_shared<Material>(m_renderer, 0, std::move(def.shader), std::move(props));
    });
}

void MaterialManager::selfRegister () {
    core::Assets::AddManager(this);
}
