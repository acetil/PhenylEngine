#pragma once

#include <cstdint>

#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/maths.h>
#include <phenyl/prefab.h>
#include <phenyl/runtime.h>
#include <phenyl/serialization_fwd.h>
#include <phenyl/ui/label.h>
#include <phenyl/ui/ui.h>

namespace breakout {
    class BreakoutApp;

    struct TileController {
        std::size_t rows{};
        std::size_t columns{};

        glm::vec2 startOffset{};
        glm::vec2 endOffset{};
        glm::vec2 labelPos{};

        phenyl::Asset<phenyl::Prefab> tile;


        BreakoutApp* app = nullptr;
        std::size_t tilesRemaining = 0;
        int points = 0;
        //phenyl::ui::Label pointsLabel{"label"};
        phenyl::ui::LabelWidget* pointsLabel = nullptr;

        TileController () = default;
        static void Init (BreakoutApp* app, phenyl::PhenylRuntime& runtime);

        void onInsert (phenyl::Entity entity, BreakoutApp* app, phenyl::UIManager& uiManager);
        void onTileBreak (int tilePoints);
    };

    struct OnTileBreak {
        int points;
    };

    PHENYL_DECLARE_SERIALIZABLE(TileController)
}
