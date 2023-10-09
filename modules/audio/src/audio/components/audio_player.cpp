#include "audio/components/audio_player.h"

using namespace phenyl::audio;

void AudioPlayer::play (const common::Asset<AudioSample>& sample) {
    source.play(sample);
}

AudioPlayer::AudioPlayer (const AudioPlayer& other) : source{} {}

AudioPlayer& AudioPlayer::operator= (const AudioPlayer& other) {
    if (this == &other) {
        return *this;
    }

    source = AudioSource{};

    return *this;
}
