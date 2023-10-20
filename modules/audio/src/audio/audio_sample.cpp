#include "audio/audio_sample.h"
#include "audio/audio_system.h"

using namespace phenyl::audio;

AudioSample::AudioSample (phenyl::audio::AudioSample&& other) noexcept : audioSystem{other.audioSystem}, sampleId{other.sampleId} {
    other.audioSystem = nullptr;
    other.sampleId = 0;
}

AudioSample& AudioSample::operator= (AudioSample&& other) noexcept {
    if (audioSystem && sampleId) {
        audioSystem->destroySample(sampleId);
    }

    audioSystem = other.audioSystem;
    sampleId = other.sampleId;

    other.audioSystem = nullptr;
    other.sampleId = 0;

    return *this;
}

AudioSample::~AudioSample () {
    if (audioSystem && sampleId) {
        audioSystem->destroySample(sampleId);
    }
}

float AudioSample::duration () const {
    return audioSystem->getSampleDuration(*this);
}
