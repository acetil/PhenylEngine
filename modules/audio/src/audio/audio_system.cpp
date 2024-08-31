#include "common/assets/assets.h"
#include "component/component.h"
#include "component/component_serializer.h"
#include "common/serialization/serializer_impl.h"

#include "audio/audio_system.h"
#include "filetypes/wav.h"
#include "openal/openal_system.h"
#include "audio/components/audio_player.h"
#include "audio/detail/loggers.h"

namespace phenyl::audio {
    PHENYL_SERIALIZABLE(AudioPlayer, PHENYL_SERIALIZABLE_MEMBER_NAMED(sourceGain, "gain"))
}

using namespace phenyl::audio;

static phenyl::Logger LOGGER{"AUDIO_SYSTEM", detail::AUDIO_LOGGER};

AudioSystem::AudioSystem (std::unique_ptr<AudioBackend> backend, std::size_t maxBackendSources) : backend{std::move(backend)} {
    PHENYL_LOGD(LOGGER, "Attempting to provision {} sources", maxBackendSources);
    backendSources.reserve(maxBackendSources);
    for (std::size_t i = 0; i < maxBackendSources; i++) {
        auto id = this->backend->makeSource();
        if (!id) {
            PHENYL_LOGE(LOGGER, "Failed to provision backend source!");
            break;
        }

        backendSources.emplace_back(BackendSource{.backendId=id});
    }

    PHENYL_LOGI(LOGGER, "Initialising audio system with {} backend sources", backendSources.size());

    sourceFreeList = 0;
    for (std::size_t i = 0; i < backendSources.size() - 1; i++) {
        backendSources[i].next = i + 1;
    }
    backendSources.back().next = EMPTY_INDEX;

    LRUHead = EMPTY_INDEX;
    LRUTail = EMPTY_INDEX;
}

AudioSystem::~AudioSystem () {
    samples.clear();
    for (auto& i : backendSources) {
        backend->destroySource(i.backendId);
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

    samples[id] = std::unique_ptr<AudioSample>(new AudioSample(this, backend->makeWAVSample(wavOpt.getUnsafe()))); // cpp is trash

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

void AudioSystem::selfRegister () {
    common::Assets::AddManager(this);
}

bool AudioSystem::isBinary () const {
    return true;
}

void AudioSystem::addComponents (component::World& world, component::EntityComponentSerializer& serializer) {
    world.addComponent<AudioPlayer>("AudioPlayer");
    serializer.addSerializer<AudioPlayer>();

    world.addHandler<AudioPlayer>([this] (const component::OnInsert<AudioPlayer>& signal, component::Entity entity) {
        auto& comp = signal.get();
        comp.source = this->createSource();
        comp.setGain(comp.sourceGain);
    });
}

AudioSource AudioSystem::createSource () {
    auto id = virtualSources.emplace();

    return {this, id + 1};
}

void AudioSystem::destroySource (std::size_t id) {
    PHENYL_DASSERT(id);
    auto virtualIndex = id - 1;
    auto curr = virtualSources[virtualIndex].sourcesStart;
    while (curr != EMPTY_INDEX) {
        auto& source = backendSources[curr];
        PHENYL_DASSERT(source.active);
        backend->stopSource(source.backendId);

        if (LRUHead == curr) {
            LRUHead = source.nextLRU;
        } else {
            PHENYL_DASSERT(source.prevLRU != EMPTY_INDEX);
            backendSources[source.prevLRU].nextLRU = source.nextLRU;
        }

        if (LRUTail == curr) {
            LRUTail = source.prevLRU;
        } else {
            PHENYL_DASSERT(source.nextLRU != EMPTY_INDEX);
            backendSources[source.nextLRU].prevLRU = source.prevLRU;
        }

        source.active = false;

        auto next = source.next;
        source.next = sourceFreeList;
        sourceFreeList = curr;

        curr = next;
    }

    virtualSources.remove(virtualIndex);
}

void AudioSystem::destroySample (std::size_t id) {
    backend->destroySample(id);
}

void AudioSystem::playSample (AudioSource& source, const AudioSample& sample) {
    auto virtualIndex = getVirtualSource(source);
    if (virtualIndex == EMPTY_INDEX) {
        //logging::log(LEVEL_ERROR, "Attempted to play to invalid source!");
        PHENYL_LOGE(LOGGER, "Attepted to play sample {} to empty source {}", sample.sampleId, source.sourceId);
        return;
    }

    auto& virtualSource = virtualSources[virtualIndex];

    auto sourceIndex = provisionSource(virtualIndex);
    auto& backendSource = backendSources[sourceIndex];

    backendSource.next = virtualSource.sourcesStart;
    backendSource.prev = EMPTY_INDEX;

    if (virtualSource.sourcesStart != EMPTY_INDEX) {
        backendSources[virtualSource.sourcesStart].prev = sourceIndex;
    }

    virtualSource.sourcesStart = sourceIndex;

    if (LRUTail == EMPTY_INDEX) {
        PHENYL_DASSERT(LRUHead == EMPTY_INDEX);
        LRUHead = sourceIndex;
        LRUTail = sourceIndex;
    } else {
        backendSources[LRUTail].nextLRU = sourceIndex;
        backendSource.prevLRU = LRUTail;
        backendSource.nextLRU = EMPTY_INDEX;
        LRUTail = sourceIndex;
    }

    backend->setSourceGain(backendSource.backendId, virtualSource.gain);
    backend->playSample(backendSource.backendId, sample.sampleId);

    backendSource.remainingDuration = sample.duration();
}

float AudioSystem::getSourceGain (const AudioSource& source) const {
    auto index = getVirtualSource(source);
    if (index == EMPTY_INDEX) {
        PHENYL_LOGE(LOGGER, "Attempted to get gain of invalid source {}!", source.sourceId);
        return 0.0f;
    }

    return virtualSources[index].gain;
}

void AudioSystem::setSourceGain (AudioSource& source, float gain) {
    auto index = getVirtualSource(source);
    if (index == EMPTY_INDEX) {
        PHENYL_LOGE(LOGGER, "Attempted to set gain of invalid source {}!", source.sourceId);
        return;
    }

    virtualSources[index].gain = gain;
    for (auto curr = virtualSources[index].sourcesStart; curr != EMPTY_INDEX; curr = backendSources[curr].next) {
        backend->setSourceGain(backendSources[curr].backendId, gain);
    }
}

std::size_t AudioSystem::provisionSource (std::size_t virtualId) {
    if (sourceFreeList != EMPTY_INDEX) {
        auto newIndex = sourceFreeList;
        sourceFreeList = backendSources[newIndex].next;

        backendSources[newIndex].active = true;
        backendSources[newIndex].ownerId = virtualId;

        return newIndex;
    }

    auto index = LRUHead;
    LRUHead = backendSources[index].nextLRU;

    if (LRUHead == EMPTY_INDEX) {
        LRUTail = EMPTY_INDEX;
    } else {
        backendSources[LRUHead].prevLRU = EMPTY_INDEX;
    }

    auto& source = backendSources[index];
    backend->stopSource(source.backendId);
    if (source.prev == EMPTY_INDEX) {
        virtualSources[source.ownerId].sourcesStart = source.next;
    } else {
        backendSources[source.prev].next = source.next;
    }

    if (source.next != EMPTY_INDEX) {
        backendSources[source.next].prev = source.prev;
    }

    backendSources[index].ownerId = virtualId;
    PHENYL_DASSERT(source.active);

    return index;
}

std::size_t AudioSystem::getVirtualSource (const AudioSource& source) const {
    if (!source) {
        return EMPTY_INDEX;
    }

    return source.sourceId - 1;
}

float AudioSystem::getSampleDuration (const AudioSample& sample) const {
    return backend->getSampleDuration(sample.sampleId);
}

void AudioSystem::update (float deltaTime) {
    for (std::size_t i = 0; i < backendSources.size(); i++) {
        auto& source = backendSources[i];
        if (!source.active) {
            continue;
        }

        source.remainingDuration -= deltaTime;
        if (source.remainingDuration >= 0.0f || !backend->isSourceStopped(source.backendId)) {
            continue;
        }

        if (source.prev == EMPTY_INDEX) {
            PHENYL_DASSERT(source.ownerId != EMPTY_INDEX);
            virtualSources[source.ownerId].sourcesStart = source.next;
        } else {
            backendSources[source.prev].next = source.next;
        }

        if (source.next != EMPTY_INDEX) {
            backendSources[source.next].prev = source.prev;
        }

        if (LRUHead == i) {
            LRUHead = source.nextLRU;
        } else {
            backendSources[source.prevLRU].nextLRU = source.nextLRU;
        }

        if (LRUTail == i) {
            LRUTail = source.prevLRU;
        } else {
            backendSources[source.nextLRU].prevLRU = source.prevLRU;
        }

        source.next = sourceFreeList;
        sourceFreeList = i;
        source.active = false;
    }
}

std::string_view AudioSystem::getName () const noexcept {
    return "AudioSystem";
}
