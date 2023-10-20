#include "logging/logging.h"

#include "audio/audio_source.h"
#include "audio/audio_system.h"

using namespace phenyl;

audio::AudioSource::~AudioSource () {
    if (audioSystem) {
        audioSystem->destroySource(sourceId);
    }
}

audio::AudioSource::AudioSource (audio::AudioSource&& other) noexcept : audioSystem{other.audioSystem}, sourceId{other.sourceId} {
    other.audioSystem = nullptr;
    other.sourceId = 0;
}

audio::AudioSource& audio::AudioSource::operator= (audio::AudioSource&& other) noexcept {
    if (audioSystem) {
        audioSystem->destroySource(sourceId);
    }
    audioSystem = other.audioSystem;
    sourceId = other.sourceId;

    other.audioSystem = nullptr;
    other.sourceId = 0;

    return *this;
}

void audio::AudioSource::play (const audio::AudioSample& sample) {
    if (!*this) {
        logging::log(LEVEL_ERROR, "Attempted to play to empty source!");
        return;
    }

    audioSystem->playSample(*this, sample);
}

void audio::AudioSource::play (const common::Asset<AudioSample>& sample) {
    if (sample) {
        play(*sample);
    } else {
        logging::log(LEVEL_ERROR, "Attempted to play invalid sample!");
    }
}

float audio::AudioSource::getGain () const {
    return audioSystem->getSourceGain(*this);
}

void audio::AudioSource::setGain (float gain) {
    audioSystem->setSourceGain(*this, gain);
}
