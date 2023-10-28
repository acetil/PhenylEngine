

module;

#include "audio/audio_sample.h"
#include "audio/audio_source.h"
#include "audio/audio_system.h"

export module phenyl.audio;

export namespace phenyl {
    using AudioSystem = audio::AudioSystem;
    using AudioSource = audio::AudioSource;
    using AudioSample = audio::AudioSample;
}