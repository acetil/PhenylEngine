#pragma once

#include "audio/audio_backend.h"
#include "audio/audio_sample.h"
#include "audio/audio_source.h"
#include "core/assets/asset_manager.h"
#include "core/iresource.h"
#include "util/fl_vector.h"

#include <memory>

namespace phenyl::core {
class World;
class EntityComponentSerializer;
} // namespace phenyl::core

namespace phenyl::audio {
class WAVFile;

class AudioSystem : public core::AssetManager<AudioSample>, public core::IResource {
public:
    explicit AudioSystem (std::unique_ptr<AudioBackend> backend, std::size_t maxBackendSources = 32);
    ~AudioSystem () override;

    std::shared_ptr<AudioSample> load (std::ifstream& data) override;
    [[nodiscard]] const char* getFileType () const override;
    bool isBinary () const override;

    // virtual AudioSource createSource () = 0;
    AudioSource createSource ();

    void selfRegister ();
    void addComponents (core::World& world, core::EntityComponentSerializer& serializer);

    void playSample (AudioSource& source, const AudioSample& sample);

    [[nodiscard]] float getSourceGain (const AudioSource& source) const;
    void setSourceGain (AudioSource& source, float gain);

    void update (float deltaTime);

    std::string_view getName () const noexcept override;

protected:
    void destroySource (std::size_t id);
    void destroySample (std::size_t id);

private:
    static constexpr std::size_t EMPTY_INDEX = static_cast<std::size_t>(-1);

    struct VirtualSource {
        float gain{1.0f};

        std::size_t sourcesStart{EMPTY_INDEX};
    };

    struct BackendSource {
        std::size_t backendId{};
        std::size_t ownerId{EMPTY_INDEX};

        std::size_t next{EMPTY_INDEX};
        std::size_t prev{EMPTY_INDEX};

        std::size_t nextLRU{EMPTY_INDEX};
        std::size_t prevLRU{EMPTY_INDEX};

        float remainingDuration{0.0f};
        bool active{false};
    };

    std::unique_ptr<AudioBackend> m_backend;

    util::FLVector<VirtualSource> m_virtualSources;
    std::vector<BackendSource> m_backendSources;

    std::size_t m_sourceFreeList;
    std::size_t m_lruHead;
    std::size_t m_lruTail;

    std::size_t provisionSource (std::size_t virtualId);
    [[nodiscard]] std::size_t getVirtualSource (const AudioSource& source) const;
    [[nodiscard]] float getSampleDuration (const AudioSample& sample) const;

    friend AudioSource;
    friend AudioSample;
};

std::unique_ptr<AudioSystem> MakeOpenALSystem ();
} // namespace phenyl::audio
