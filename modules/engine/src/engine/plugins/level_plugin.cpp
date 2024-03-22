#include "engine/level/level_manager.h"
#include "engine/plugins/level_plugin.h"

using namespace phenyl::engine;

LevelPlugin::LevelPlugin () = default;
LevelPlugin::~LevelPlugin () = default;


std::string_view LevelPlugin::getName () const noexcept {
    return "LevelPlugin";
}

void LevelPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addResource<game::LevelManager>(runtime.manager(), runtime.serializer());
    runtime.resource<game::LevelManager>().selfRegister();
}