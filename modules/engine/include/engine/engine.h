#pragma once

#include <memory>

#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"

namespace engine {

    namespace detail {
        class Engine;
    }

    class PhenylEngine {
    private:
        std::unique_ptr<detail::Engine> internal;
    public:
        PhenylEngine ();
        ~PhenylEngine();

        game::GameObject::SharedPtr getGame ();

        graphics::PhenylGraphics getGraphics ();
    };
}