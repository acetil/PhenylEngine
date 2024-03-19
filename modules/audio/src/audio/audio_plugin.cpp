#include "audio/audio_system.h"
#include "audio/audio_plugin.h"

using namespace phenyl::audio;

AudioPlugin::AudioPlugin () : audioSystem(MakeOpenALSystem()) {}
AudioPlugin::~AudioPlugin () = default;

std::string_view AudioPlugin::getName () const noexcept {
    return "AudioPlugin";
}

void AudioPlugin::init (runtime::PhenylRuntime& runtime) {
    audioSystem->selfRegister();
    audioSystem->addComponents(runtime.manager(), runtime.serializer());
}

void AudioPlugin::update (phenyl::runtime::PhenylRuntime& runtime, double deltaTime) {
    audioSystem->update((float)deltaTime);
}


