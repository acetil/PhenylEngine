#include <phenyl/serialization.h>

#include "controller.h"

#include <phenyl/component.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/signals/lifecycle.h>

#include "breakout.h"

namespace breakout {
    PHENYL_SERIALIZABLE(TileController,
        PHENYL_SERIALIZABLE_MEMBER(rows),
        PHENYL_SERIALIZABLE_MEMBER(columns),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(startOffset, "start_offset"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(endOffset, "end_offset"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(labelPos, "label_pos"),
        PHENYL_SERIALIZABLE_MEMBER(tile)
    )
}

using namespace breakout;

TileController::TileController (const TileController& other) : rows{other.rows}, columns(other.columns), startOffset(other.startOffset), endOffset(other.endOffset), labelPos(other.labelPos), tile(other.tile), tilesRemaining(other.tilesRemaining), points(other.points) {}

TileController& TileController::operator= (const TileController& other) {
    if (&other == this) {
        return *this;
    }

    rows = other.rows;
    columns = other.columns;
    startOffset = other.startOffset;
    endOffset = other.endOffset;
    labelPos = other.labelPos;
    tile = other.tile;
    tilesRemaining = other.tilesRemaining;

    return *this;
}

TileController& TileController::operator= (TileController&& other) noexcept {
    rows = other.rows;
    columns = other.columns;
    startOffset = other.startOffset;
    endOffset = other.endOffset;
    labelPos = other.labelPos;
    tile = std::move(other.tile);
    app = other.app;
    tilesRemaining = other.tilesRemaining;
    points = other.points;
    pointsLabel = std::move(other.pointsLabel);

    return *this;
}

void TileController::Init (BreakoutApp* app, phenyl::PhenylRuntime& runtime) {
    app->addComponent<TileController>("TileController");

    auto& uiManager = runtime.resource<phenyl::UIManager>();
    runtime.world().addHandler<TileController>([&uiManager, app] (const phenyl::signals::OnInsert<TileController>& signal, phenyl::Entity entity) {
        signal.get().onInsert(entity, app, uiManager);
    });

    runtime.world().addHandler<OnTileBreak, TileController>([] (const OnTileBreak& signal, TileController& controller) {
        controller.onTileBreak(signal.points);
    });
}

void TileController::onInsert (phenyl::Entity entity, BreakoutApp* app, phenyl::UIManager& uiManager) {
    this->app = app;

    float xIncrement = (endOffset.x - startOffset.x) / static_cast<float>(columns);
    float yIncrement = (endOffset.y - startOffset.y) / static_cast<float>(rows);

    for (std::size_t i = 0; i < rows; i++) {
        glm::vec2 pos = startOffset + glm::vec2{0, yIncrement} * static_cast<float>(i);
        for (std::size_t j = 0; j < columns; j++) {
            auto tileEntity = entity.createChild();
            tile->instantiate(tileEntity);
            tileEntity.apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(pos);
            });

            pos += glm::vec2{xIncrement, 0};
        }
    }

    tilesRemaining = rows * columns;
    pointsLabel.text = std::format("Points: {}", points);
    uiManager.addUIComp(pointsLabel, labelPos);
}

void TileController::onTileBreak (int tilePoints) {
    points += tilePoints;
    pointsLabel.text = std::format("Points: {}", points);

    if (!(--tilesRemaining)) {
        app->onWin();
    }
}
