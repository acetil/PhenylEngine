#include "common/assets/assets.h"
#include "component/component.h"
#include "component/component_serializer.h"
#include "component/signals/component_update.h"

#include "audio/audio_system.h"
#include "filetypes/wav.h"
#include "openal/openal_system.h"
#include "audio/components/audio_player.h"

namespace phenyl::audio {
    PHENYL_SERIALIZE(AudioPlayer, {})
}

using namespace phenyl::audio;

std::unique_ptr<AudioSystem> phenyl::audio::MakeOpenALSystem () {
    return std::make_unique<OpenALSystem>();
}

AudioSource AudioSystem::makeSource (std::size_t id) {
    return {this, id};
}

AudioSample AudioSystem::makeSample (std::size_t id) {
    return {this, id};
}

AudioSample* AudioSystem::load (std::istream& data, std::size_t id) {
    // Assume is wav format
    auto wavOpt = WAVFile::Load(data);
    if (!wavOpt) {
        return nullptr;
    }

    samples[id] = std::make_unique<AudioSample>(makeWAVSample(wavOpt.getUnsafe()));

    return samples[id].get();
}

AudioSample* AudioSystem::load (AudioSample&& obj, std::size_t id) {
    samples[id] = std::make_unique<AudioSample>(std::move(obj));

    return samples[id].get();
}

void AudioSystem::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        samples.remove(id);
    }
}

const char* AudioSystem::getFileType () const {
    return "";
}

void AudioSystem::cleanup () {
    samples.clear();
}

void AudioSystem::selfRegister () {
    common::Assets::AddManager(this);
}

bool AudioSystem::isBinary () const {
    return true;
}

void AudioSystem::addComponents (component::ComponentManager& manager, component::EntitySerializer& serializer) {
    manager.addComponent<AudioPlayer>();
    serializer.addSerializer<AudioPlayer>();

    manager.handleSignal<component::OnInsert<AudioPlayer>>([this] (auto entity, const component::OnInsert<AudioPlayer>& signal) {
        signal.get().source = this->createSource();
    });
}
