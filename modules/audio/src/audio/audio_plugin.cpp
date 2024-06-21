#include "audio/audio_system.h"
#include "audio/audio_plugin.h"

#include "runtime/delta_time.h"

using namespace phenyl::audio;

static void AudioUpdateSystem (const phenyl::runtime::Resources<AudioSystem, phenyl::runtime::DeltaTime>& resources) {
    auto& [system, deltaTime] = resources;
    system.update(static_cast<float>(deltaTime()));
}

AudioPlugin::AudioPlugin () : audioSystem(MakeOpenALSystem()) {}
AudioPlugin::~AudioPlugin () = default;

std::string_view AudioPlugin::getName () const noexcept {
    return "AudioPlugin";
}

void AudioPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addResource<AudioSystem>(audioSystem.get());

    audioSystem->selfRegister();
    audioSystem->addComponents(runtime.manager(), runtime.serializer());

    runtime.addSystem<phenyl::runtime::Update>(AudioUpdateSystem);
}

void AudioPlugin::update (phenyl::runtime::PhenylRuntime& runtime, double deltaTime) {
    //audioSystem->update((float)deltaTime);
}


