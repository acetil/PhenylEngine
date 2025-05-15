#pragma once

#include "core/plugin.h"

#include <memory>

namespace phenyl::audio {
class AudioSystem;

class AudioPlugin : public core::IPlugin {
public:
    AudioPlugin ();
    ~AudioPlugin () override;

    std::string_view getName () const noexcept override;

    void init (core::PhenylRuntime& runtime) override;

private:
    std::unique_ptr<AudioSystem> m_audioSystem;
};
} // namespace phenyl::audio
