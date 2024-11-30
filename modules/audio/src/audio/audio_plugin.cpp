#include "audio/audio_system.h"
#include "audio/audio_plugin.h"

#include "core/delta_time.h"
#include "core/runtime.h"

using namespace phenyl::audio;

static void AudioUpdateSystem (const phenyl::core::Resources<AudioSystem, phenyl::core::DeltaTime>& resources) {
    auto& [system, deltaTime] = resources;
    system.update(static_cast<float>(deltaTime()));
}

AudioPlugin::AudioPlugin () : audioSystem(MakeOpenALSystem()) {}
AudioPlugin::~AudioPlugin () = default;

std::string_view AudioPlugin::getName () const noexcept {
    return "AudioPlugin";
}

void AudioPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addResource<AudioSystem>(audioSystem.get());

    audioSystem->selfRegister();
    audioSystem->addComponents(runtime.world(), runtime.serializer());

    runtime.addSystem<phenyl::core::Update>("AudioSystem::Update", AudioUpdateSystem);
}


