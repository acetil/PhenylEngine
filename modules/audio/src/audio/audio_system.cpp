#include "audio/audio_system.h"

#include "audio/components/audio_player.h"
#include "audio/detail/loggers.h"
#include "core/assets/assets.h"
#include "core/serialization/component_serializer.h"
#include "core/serialization/serializer_impl.h"
#include "core/world.h"
#include "filetypes/wav.h"
#include "openal/openal_system.h"

namespace phenyl::audio {
PHENYL_SERIALIZABLE(AudioPlayer, PHENYL_SERIALIZABLE_MEMBER_NAMED(m_gain, "gain"))
}

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AUDIO_SYSTEM", detail::AUDIO_LOGGER};

AudioSystem::AudioSystem (std::unique_ptr<AudioBackend> backend, std::size_t maxBackendSources) :
    m_backend{std::move(backend)} {
    PHENYL_LOGD(LOGGER, "Attempting to provision {} sources", maxBackendSources);
    m_backendSources.reserve(maxBackendSources);
    for (std::size_t i = 0; i < maxBackendSources; i++) {
        auto id = this->m_backend->makeSource();
        if (!id) {
            PHENYL_LOGE(LOGGER, "Failed to provision backend source!");
            break;
        }

        m_backendSources.emplace_back(BackendSource{.backendId = id});
    }

    PHENYL_LOGI(LOGGER, "Initialising audio system with {} backend sources", m_backendSources.size());

    m_sourceFreeList = 0;
    for (std::size_t i = 0; i < m_backendSources.size() - 1; i++) {
        m_backendSources[i].next = i + 1;
    }
    m_backendSources.back().next = EMPTY_INDEX;

    m_lruHead = EMPTY_INDEX;
    m_lruTail = EMPTY_INDEX;
}

AudioSystem::~AudioSystem () {
    m_samples.clear();
    for (auto& i : m_backendSources) {
        m_backend->destroySource(i.backendId);
    }
}

std::unique_ptr<AudioSystem> phenyl::audio::MakeOpenALSystem () {
    return std::make_unique<AudioSystem>(std::make_unique<OpenALSystem>());
}

AudioSample* AudioSystem::load (std::ifstream& data, std::size_t id) {
    // Assume is wav format
    auto wavOpt = WAVFile::Load(data);
    if (!wavOpt) {
        return nullptr;
    }

    m_samples[id] =
        std::unique_ptr<AudioSample>(new AudioSample(this, m_backend->makeWAVSample(*wavOpt))); // cpp is trash

    return m_samples[id].get();
}

AudioSample* AudioSystem::load (AudioSample&& obj, std::size_t id) {
    m_samples[id] = std::make_unique<AudioSample>(std::move(obj));

    return m_samples[id].get();
}

void AudioSystem::queueUnload (std::size_t id) {
    if (onUnload(id)) {
        m_samples.erase(id);
    }
}

const char* AudioSystem::getFileType () const {
    return "";
}

void AudioSystem::selfRegister () {
    core::Assets::AddManager(this);
}

bool AudioSystem::isBinary () const {
    return true;
}

void AudioSystem::addComponents (core::World& world, core::EntityComponentSerializer& serializer) {
    world.addComponent<AudioPlayer>("AudioPlayer");
    serializer.addSerializer<AudioPlayer>();

    world.addHandler<AudioPlayer>([this] (const core::OnInsert<AudioPlayer>& signal, core::Entity entity) {
        auto& comp = signal.get();
        comp.m_source = this->createSource();
        comp.setGain(comp.m_gain);
    });
}

AudioSource AudioSystem::createSource () {
    auto id = m_virtualSources.emplace();

    return {this, id + 1};
}

void AudioSystem::destroySource (std::size_t id) {
    PHENYL_DASSERT(id);
    auto virtualIndex = id - 1;
    auto curr = m_virtualSources[virtualIndex].sourcesStart;
    while (curr != EMPTY_INDEX) {
        auto& source = m_backendSources[curr];
        PHENYL_DASSERT(source.active);
        m_backend->stopSource(source.backendId);

        if (m_lruHead == curr) {
            m_lruHead = source.nextLRU;
        } else {
            PHENYL_DASSERT(source.prevLRU != EMPTY_INDEX);
            m_backendSources[source.prevLRU].nextLRU = source.nextLRU;
        }

        if (m_lruTail == curr) {
            m_lruTail = source.prevLRU;
        } else {
            PHENYL_DASSERT(source.nextLRU != EMPTY_INDEX);
            m_backendSources[source.nextLRU].prevLRU = source.prevLRU;
        }

        source.active = false;

        auto next = source.next;
        source.next = m_sourceFreeList;
        m_sourceFreeList = curr;

        curr = next;
    }

    m_virtualSources.remove(virtualIndex);
}

void AudioSystem::destroySample (std::size_t id) {
    m_backend->destroySample(id);
}

void AudioSystem::playSample (AudioSource& source, const AudioSample& sample) {
    auto virtualIndex = getVirtualSource(source);
    if (virtualIndex == EMPTY_INDEX) {
        // logging::log(LEVEL_ERROR, "Attempted to play to invalid source!");
        PHENYL_LOGE(LOGGER, "Attepted to play sample {} to empty source {}", sample.m_id, source.m_id);
        return;
    }

    auto& virtualSource = m_virtualSources[virtualIndex];

    auto sourceIndex = provisionSource(virtualIndex);
    auto& backendSource = m_backendSources[sourceIndex];

    backendSource.next = virtualSource.sourcesStart;
    backendSource.prev = EMPTY_INDEX;

    if (virtualSource.sourcesStart != EMPTY_INDEX) {
        m_backendSources[virtualSource.sourcesStart].prev = sourceIndex;
    }

    virtualSource.sourcesStart = sourceIndex;

    if (m_lruTail == EMPTY_INDEX) {
        PHENYL_DASSERT(m_lruHead == EMPTY_INDEX);
        m_lruHead = sourceIndex;
        m_lruTail = sourceIndex;
    } else {
        m_backendSources[m_lruTail].nextLRU = sourceIndex;
        backendSource.prevLRU = m_lruTail;
        backendSource.nextLRU = EMPTY_INDEX;
        m_lruTail = sourceIndex;
    }

    m_backend->setSourceGain(backendSource.backendId, virtualSource.gain);
    m_backend->playSample(backendSource.backendId, sample.m_id);

    backendSource.remainingDuration = sample.duration();
}

float AudioSystem::getSourceGain (const AudioSource& source) const {
    auto index = getVirtualSource(source);
    if (index == EMPTY_INDEX) {
        PHENYL_LOGE(LOGGER, "Attempted to get gain of invalid source {}!", source.m_id);
        return 0.0f;
    }

    return m_virtualSources[index].gain;
}

void AudioSystem::setSourceGain (AudioSource& source, float gain) {
    auto index = getVirtualSource(source);
    if (index == EMPTY_INDEX) {
        PHENYL_LOGE(LOGGER, "Attempted to set gain of invalid source {}!", source.m_id);
        return;
    }

    m_virtualSources[index].gain = gain;
    for (auto curr = m_virtualSources[index].sourcesStart; curr != EMPTY_INDEX; curr = m_backendSources[curr].next) {
        m_backend->setSourceGain(m_backendSources[curr].backendId, gain);
    }
}

std::size_t AudioSystem::provisionSource (std::size_t virtualId) {
    if (m_sourceFreeList != EMPTY_INDEX) {
        auto newIndex = m_sourceFreeList;
        m_sourceFreeList = m_backendSources[newIndex].next;

        m_backendSources[newIndex].active = true;
        m_backendSources[newIndex].ownerId = virtualId;

        return newIndex;
    }

    auto index = m_lruHead;
    m_lruHead = m_backendSources[index].nextLRU;

    if (m_lruHead == EMPTY_INDEX) {
        m_lruTail = EMPTY_INDEX;
    } else {
        m_backendSources[m_lruHead].prevLRU = EMPTY_INDEX;
    }

    auto& source = m_backendSources[index];
    m_backend->stopSource(source.backendId);
    if (source.prev == EMPTY_INDEX) {
        m_virtualSources[source.ownerId].sourcesStart = source.next;
    } else {
        m_backendSources[source.prev].next = source.next;
    }

    if (source.next != EMPTY_INDEX) {
        m_backendSources[source.next].prev = source.prev;
    }

    m_backendSources[index].ownerId = virtualId;
    PHENYL_DASSERT(source.active);

    return index;
}

std::size_t AudioSystem::getVirtualSource (const AudioSource& source) const {
    if (!source) {
        return EMPTY_INDEX;
    }

    return source.m_id - 1;
}

float AudioSystem::getSampleDuration (const AudioSample& sample) const {
    return m_backend->getSampleDuration(sample.m_id);
}

void AudioSystem::update (float deltaTime) {
    for (std::size_t i = 0; i < m_backendSources.size(); i++) {
        auto& source = m_backendSources[i];
        if (!source.active) {
            continue;
        }

        source.remainingDuration -= deltaTime;
        if (source.remainingDuration >= 0.0f || !m_backend->isSourceStopped(source.backendId)) {
            continue;
        }

        if (source.prev == EMPTY_INDEX) {
            PHENYL_DASSERT(source.ownerId != EMPTY_INDEX);
            m_virtualSources[source.ownerId].sourcesStart = source.next;
        } else {
            m_backendSources[source.prev].next = source.next;
        }

        if (source.next != EMPTY_INDEX) {
            m_backendSources[source.next].prev = source.prev;
        }

        if (m_lruHead == i) {
            m_lruHead = source.nextLRU;
        } else {
            m_backendSources[source.prevLRU].nextLRU = source.nextLRU;
        }

        if (m_lruTail == i) {
            m_lruTail = source.prevLRU;
        } else {
            m_backendSources[source.nextLRU].prevLRU = source.prevLRU;
        }

        source.next = m_sourceFreeList;
        m_sourceFreeList = i;
        source.active = false;
    }
}

std::string_view AudioSystem::getName () const noexcept {
    return "AudioSystem";
}
