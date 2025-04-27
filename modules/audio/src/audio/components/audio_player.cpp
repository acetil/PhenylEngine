#include "audio/components/audio_player.h"

using namespace phenyl::audio;

void AudioPlayer::play (const core::Asset<AudioSample>& sample) {
    m_source.play(sample);
}

AudioPlayer::AudioPlayer (const AudioPlayer& other) : m_source{}, m_gain{other.m_gain} {}

AudioPlayer& AudioPlayer::operator= (const AudioPlayer& other) {
    if (this == &other) {
        return *this;
    }

    m_source = AudioSource{};
    m_gain = other.m_gain;

    return *this;
}

float AudioPlayer::gain () const {
    return m_gain;
}

void AudioPlayer::setGain (float gain) {
    m_source.setGain(gain);
    m_gain = m_source.getGain();
}
