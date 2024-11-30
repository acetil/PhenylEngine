#include "audio/components/audio_player.h"

using namespace phenyl::audio;

void AudioPlayer::play (const core::Asset<AudioSample>& sample) {
    source.play(sample);
}

AudioPlayer::AudioPlayer (const AudioPlayer& other) : source{}, sourceGain{other.sourceGain} {}

AudioPlayer& AudioPlayer::operator= (const AudioPlayer& other) {
    if (this == &other) {
        return *this;
    }

    source = AudioSource{};
    sourceGain = other.sourceGain;

    return *this;
}

float AudioPlayer::gain () const {
    return sourceGain;
}

void AudioPlayer::setGain (float gain) {
    source.setGain(gain);
    sourceGain = source.getGain();
}
