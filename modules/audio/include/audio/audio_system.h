#pragma once

#include <memory>

#include "common/assets/asset_manager.h"
#include "util/map.h"

#include "audio_source.h"
#include "audio_sample.h"

namespace phenyl::component {
    class ComponentManager;
    class EntitySerializer;
}

namespace phenyl::audio {
    class WAVFile;

    class AudioSystem : public common::AssetManager<AudioSample> {
    private:
        util::Map<std::size_t, std::unique_ptr<AudioSample>> samples;
    protected:
        AudioSource makeSource (std::size_t id);
        AudioSample makeSample (std::size_t id);

        virtual AudioSample makeWAVSample (const WAVFile& wavFile) = 0;

        virtual void playSample (std::size_t sourceId, std::size_t sampleId) = 0;
        virtual void destroySource (std::size_t id) = 0;
        virtual void destroySample (std::size_t id) = 0;

        [[nodiscard]] virtual float getSourceGain (std::size_t id) const = 0;
        virtual void setSourceGain (std::size_t id, float gain) = 0;

        void cleanup ();

        friend AudioSource;
        friend AudioSample;
    public:
        ~AudioSystem() override = default;

        AudioSample* load (std::istream &data, std::size_t id) override;
        AudioSample* load (phenyl::audio::AudioSample &&obj, std::size_t id) override;
        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
        bool isBinary() const override;

        virtual AudioSource createSource () = 0;

        void selfRegister ();
        void addComponents (component::ComponentManager& manager, component::EntitySerializer& serializer);
    };

    std::unique_ptr<AudioSystem> MakeOpenALSystem ();
}