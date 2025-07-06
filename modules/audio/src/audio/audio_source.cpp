#include "audio/audio_source.h"

#include "audio/audio_system.h"
#include "audio/detail/loggers.h"
#include "logging/logging.h"

using namespace phenyl;

static Logger LOGGER{"AUDIO_SOURCE", audio::detail::AUDIO_LOGGER};

audio::AudioSource::~AudioSource () {
    if (m_audioSystem) {
        m_audioSystem->destroySource(m_id);
    }
}

audio::AudioSource::AudioSource (audio::AudioSource&& other) noexcept :
    m_audioSystem{other.m_audioSystem},
    m_id{other.m_id} {
    other.m_audioSystem = nullptr;
    other.m_id = 0;
}

audio::AudioSource& audio::AudioSource::operator= (audio::AudioSource&& other) noexcept {
    if (m_audioSystem) {
        m_audioSystem->destroySource(m_id);
    }
    m_audioSystem = other.m_audioSystem;
    m_id = other.m_id;

    other.m_audioSystem = nullptr;
    other.m_id = 0;

    return *this;
}

void audio::AudioSource::play (const audio::AudioSample& sample) {
    PHENYL_ASSERT_MSG(*this, "Attempted to play to empty source");

    m_audioSystem->playSample(*this, sample);
}

float audio::AudioSource::getGain () const {
    return m_audioSystem->getSourceGain(*this);
}

void audio::AudioSource::setGain (float gain) {
    m_audioSystem->setSourceGain(*this, gain);
}
