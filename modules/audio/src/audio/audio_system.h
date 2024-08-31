#pragma once

#include <memory>

#include "common/assets/asset_manager.h"
#include "util/map.h"
#include "util/fl_vector.h"

#include "audio/audio_source.h"
#include "audio/audio_sample.h"
#include "audio/audio_backend.h"
#include "runtime/iresource.h"

namespace phenyl::component {
    class World;
    class EntityComponentSerializer;
}

namespace phenyl::audio {
    class WAVFile;

    class AudioSystem : public common::AssetManager<AudioSample>, public runtime::IResource {
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

        util::Map<std::size_t, std::unique_ptr<AudioSample>> samples;
        std::unique_ptr<AudioBackend> backend;

        util::FLVector<VirtualSource> virtualSources;
        std::vector<BackendSource> backendSources;

        std::size_t sourceFreeList;
        std::size_t LRUHead;
        std::size_t LRUTail;


        std::size_t provisionSource (std::size_t virtualId);
        [[nodiscard]] std::size_t getVirtualSource (const AudioSource& source) const;
        [[nodiscard]] float getSampleDuration (const AudioSample& sample) const;
    protected:
        void destroySource (std::size_t id);
        void destroySample (std::size_t id);

        friend AudioSource;
        friend AudioSample;
    public:
        explicit AudioSystem (std::unique_ptr<AudioBackend> backend, std::size_t maxBackendSources=32);
        ~AudioSystem() override;

        AudioSample* load (std::ifstream &data, std::size_t id) override;
        AudioSample* load (phenyl::audio::AudioSample &&obj, std::size_t id) override;
        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
        bool isBinary() const override;

        //virtual AudioSource createSource () = 0;
        AudioSource createSource ();

        void selfRegister ();
        void addComponents (component::World& world, component::EntityComponentSerializer& serializer);

        void playSample (AudioSource& source, const AudioSample& sample);

        [[nodiscard]] float getSourceGain (const AudioSource& source) const;
        void setSourceGain (AudioSource& source, float gain);

        void update (float deltaTime);

        std::string_view getName () const noexcept override;
    };

    std::unique_ptr<AudioSystem> MakeOpenALSystem ();
}