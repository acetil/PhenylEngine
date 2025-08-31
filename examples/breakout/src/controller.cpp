#include "controller.h"

#include "breakout.h"
#include "phenyl/font.h"
#include "phenyl/ui/types.h"

#include <phenyl/components/2D/global_transform.h>
#include <phenyl/serialization.h>
#include <phenyl/signals/lifecycle.h>
#include <phenyl/world.h>

namespace breakout {
PHENYL_SERIALIZABLE(TileController, PHENYL_SERIALIZABLE_MEMBER(rows), PHENYL_SERIALIZABLE_MEMBER(columns),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(startOffset, "start_offset"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(endOffset, "end_offset"), PHENYL_SERIALIZABLE_MEMBER_NAMED(labelPos, "label_pos"),
    PHENYL_SERIALIZABLE_MEMBER(tile))
}

using namespace breakout;

void TileController::Init (BreakoutApp* app, phenyl::PhenylRuntime& runtime) {
    app->addComponent<TileController>("TileController");

    auto& ui = runtime.resource<phenyl::UI>();
    runtime.world().addHandler<TileController>([&ui, app] (const phenyl::signals::OnInsert<TileController>& signal,
                                                   phenyl::Entity entity) { signal.get().onInsert(entity, app, ui); });

    runtime.world().addHandler<OnTileBreak, TileController>(
        [] (const OnTileBreak& signal, TileController& controller) { controller.onTileBreak(signal.points); });
}

void TileController::onInsert (phenyl::Entity entity, BreakoutApp* app, phenyl::UI& ui) {
    this->app = app;

    float xIncrement = (endOffset.x - startOffset.x) / static_cast<float>(columns);
    float yIncrement = (endOffset.y - startOffset.y) / static_cast<float>(rows);

    for (std::size_t i = 0; i < rows; i++) {
        glm::vec2 pos = startOffset + glm::vec2{0, yIncrement} * static_cast<float>(i);
        for (std::size_t j = 0; j < columns; j++) {
            auto tileEntity = entity.createChild();
            tile->instantiate(tileEntity);
            tileEntity.apply<phenyl::Transform2D>(
                [pos] (phenyl::Transform2D& transform) { transform.setPosition(pos); });

            pos += glm::vec2{xIncrement, 0};
        }
    }

    tilesRemaining = rows * columns;
    // pointsLabel->setText(std::format("Points: {}", points);
    // pointsLabel = ui.root().add<phenyl::ui::LabelWidget>(std::format("Points: {}", points), 14,
    //     phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
    //     phenyl::ui::Modifier{}.withOffset({180, 30}));
    pointsLabel = phenyl::ui::Atom<std::string>::Make(std::format("Points: {}", points));
    ui.root().add<phenyl::ui::DynamicLabel>(phenyl::ui::DynamicLabelProps{
      .text = pointsLabel,
      .textSize = 14,
      .font = phenyl::Assets::Load<phenyl::Font>("resources/phenyl/fonts/noto-serif"),
      .modifier = phenyl::ui::Modifier{}.withOffset({180, 30}),
    });
    // uiManager.addUIComp(pointsLabel, labelPos);
}

void TileController::onTileBreak (int tilePoints) {
    points += tilePoints;
    pointsLabel.set(std::format("Points: {}", points));

    if (!(--tilesRemaining)) {
        app->onWin();
    }
}
