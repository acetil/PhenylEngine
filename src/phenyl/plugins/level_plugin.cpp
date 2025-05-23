#include "phenyl/plugins/level_plugin.h"

#include "engine/level/level_manager.h"
#include "phenyl/plugins/prefab_plugin.h"

using namespace phenyl;

LevelPlugin::LevelPlugin () = default;
LevelPlugin::~LevelPlugin () = default;

std::string_view LevelPlugin::getName () const noexcept {
    return "LevelPlugin";
}

void LevelPlugin::init (PhenylRuntime& runtime) {
    runtime.addPlugin<PrefabPlugin>();

    runtime.addResource<game::LevelManager>(runtime.world(), runtime.serializer());
    auto& levelManager = runtime.resource<game::LevelManager>();

    levelManager.selfRegister();
    runtime.addSystem<FrameBegin>("LevelManager::loadLevels", &levelManager, &game::LevelManager::loadLevels);
}
