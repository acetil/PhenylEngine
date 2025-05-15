#include "audio/audio_sample.h"

#include "audio/audio_system.h"

using namespace phenyl::audio;

AudioSample::AudioSample (phenyl::audio::AudioSample&& other) noexcept :
    m_audioSystem{other.m_audioSystem},
    m_id{other.m_id} {
    other.m_audioSystem = nullptr;
    other.m_id = 0;
}

AudioSample& AudioSample::operator= (AudioSample&& other) noexcept {
    if (m_audioSystem && m_id) {
        m_audioSystem->destroySample(m_id);
    }

    m_audioSystem = other.m_audioSystem;
    m_id = other.m_id;

    other.m_audioSystem = nullptr;
    other.m_id = 0;

    return *this;
}

AudioSample::~AudioSample () {
    if (m_audioSystem && m_id) {
        m_audioSystem->destroySample(m_id);
    }
}

float AudioSample::duration () const {
    return m_audioSystem->getSampleDuration(*this);
}
