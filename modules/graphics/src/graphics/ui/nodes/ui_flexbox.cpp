#include "graphics/ui/nodes/ui_flexbox.h"
#include "graphics/ui/ui_manager.h"
#include "graphics/ui/ui_data.h"

#include <algorithm>
#include <utility>
#include "logging/logging.h"

using namespace phenyl::graphics::ui;

namespace phenyl::graphics::ui::detail {
    struct FlexBoxItem {
        UIComponentNode* node;
        glm::vec2 offset;
        glm::vec2 size;
        glm::vec2 bounds;
    };
}

static bool canExpand (glm::vec2 size, glm::vec2 axis, const UIAnchor& anchor) {
    return glm::dot(glm::abs(axis), anchor.maximumSize) == -1 || glm::abs(glm::dot(size, axis)) < glm::abs(glm::dot(anchor.maximumSize, axis));
}

static float expandIfPossible (detail::FlexBoxItem& item, glm::vec2 axis, float sizeIncr, const UIAnchor& anchor) {
    float sizeUsed = 0;
    if (canExpand(item.size, axis, anchor)) {
        float maxIncr;
        if (glm::dot(glm::abs(axis), anchor.maximumSize) == -1) {
            maxIncr = sizeIncr;
        } else {
            maxIncr = glm::min(glm::dot(anchor.maximumSize - item.size, glm::abs(axis)),
                               sizeIncr);
        }

        item.size += maxIncr * glm::abs(axis);

        sizeUsed = maxIncr;
    }
    return sizeUsed;
}

static void expandSecondaryAxis (detail::FlexBoxItem& item, glm::vec2 axis, float axisSize, const UIAnchor& anchor) {
    float remainingSize = glm::max(0.0f, axisSize - glm::abs(glm::dot(axis, item.bounds)));

    float expandSize;

    if (glm::dot(anchor.maximumSize, glm::abs(axis)) == -1) {
        expandSize = remainingSize;
    } else {
        expandSize = glm::max(0.0f, glm::min(remainingSize, glm::dot(anchor.maximumSize, glm::abs(axis)) - glm::dot(item.size, glm::abs(axis))));
    }

    item.size += glm::abs(axis) * expandSize;
    item.bounds += glm::abs(axis) * expandSize;
}

static void alignItem (detail::FlexBoxItem& item, glm::vec2 axis, float axisSize, FloatAnchor floatAnchor) {
    float remainingSize = axisSize - glm::abs(glm::dot(axis, item.bounds));
    switch (floatAnchor) {
        case FloatAnchor::FLOAT_CENTRE:
            item.offset += axis * (remainingSize / 2);
            break;
        case FloatAnchor::FLOAT_END:
            item.offset += axis * remainingSize;
            break;
        default:
            break;
    }
}

void UIFlexBoxNode::setAxes (Axis primaryAxis, Axis secondaryAxis) {
    auto primaryDir = getAxisVec(primaryAxis);
    auto secondaryDir = getAxisVec(secondaryAxis);

    if (glm::dot(primaryDir, secondaryDir) != 0) {
        graphics::logging::log(LEVEL_ERROR, "Flexbox axes must be perpendicular!");
        return;
    }

    primaryAxisVec = primaryDir;
    secondaryAxisVec = secondaryDir;

    updateLayout();
}

UIFlexBoxNode::UIFlexBoxNode (const std::string& themeClass) : UIContainerNode(themeClass) {

}

void UIFlexBoxNode::addComponent (const std::shared_ptr<UIComponentNode>& uiNode) {
    std::shared_ptr<UIComponentNode> node = uiNode;
    addChild(node);
    children.emplace_back(std::move(node));
    updateLayout();
}

void UIFlexBoxNode::addComponent (const std::shared_ptr<UIComponentNode>& uiNode, int index) {
    std::shared_ptr<UIComponentNode> node = uiNode;
    addChild(node);
    children.emplace(children.begin() + index, node);
    updateLayout();
}

void UIFlexBoxNode::updateLayout () {
    items.clear();
    items.reserve(children.size());

    float primaryAxisSize = glm::abs(glm::dot(size, primaryAxisVec));

    float minPrimarySize = 0;
    float minSecondarySize = 0;

    std::vector<UIAnchor> anchors;

    for (auto& i : children) {
        auto curr = i->getAnchor();
        anchors.emplace_back(curr);

        minPrimarySize += glm::abs(glm::dot(curr.minimumSize + curr.topLeftMargin + curr.bottomRightMargin, primaryAxisVec));
        minSecondarySize = glm::max(glm::abs(glm::dot(curr.minimumSize + curr.topLeftMargin + curr.bottomRightMargin, secondaryAxisVec)), minSecondarySize);
    }

    componentMinSize = minPrimarySize * primaryAxisVec + minSecondarySize * secondaryAxisVec;
    componentMinSize = {glm::abs(componentMinSize.x), glm::abs(componentMinSize.y)};

    placeChildren(anchors);

    float remainingSize = expandChildren(anchors, primaryAxisSize - minPrimarySize);
    justifyChildren(remainingSize);
    alignChildren(anchors);

    auto axes = primaryAxisVec + secondaryAxisVec;

    for (auto& i : items) {
        if (axes.x < 0) {
            i.offset.x = size.x + i.offset.x - i.size.x;
        }

        if (axes.y < 0) {
            i.offset.y = size.y + i.offset.y - i.size.y;
        }
    }

    for (auto& i : items) {
        i.node->setSize(i.size);
    }

}

void UIFlexBoxNode::render (graphics::UIManager& uiManager) {
    for (auto& i : items) {
        uiManager.pushOffset(i.offset);
        i.node->render(uiManager);
        uiManager.popOffset();
    }

    updateLayout(); // TODO: update only when items update

    uiManager.renderRect({0, 0}, size, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 1.0f);
    uiManager.renderRect({0,0}, componentMinSize, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, 0.0f, 1.0f);
}

UIAnchor UIFlexBoxNode::getAnchor () {
    return UIAnchor()
        .withMinimumSize({glm::max(componentMinSize.x, minSize.x), glm::max(componentMinSize.y, minSize.y)})
        .withMaximumSize(maxSize);
}

void UIFlexBoxNode::setMaxSize (glm::vec2 _maxSize) {
    maxSize = _maxSize;
}

void UIFlexBoxNode::onMousePosChange (glm::vec2 oldMousePos) {
    for (auto& i : items) {
        i.node->setMousePos(getMousePos() - i.offset);
    }
}

bool UIFlexBoxNode::onMousePress () {
    for (auto& i : items) {
        if (i.node->onMousePress()) {
            return true;
        }
    }
    return false;
}

void UIFlexBoxNode::onMouseRelease () {
    for (auto& i : items) {
        i.node->onMouseRelease();
    }
}

void UIFlexBoxNode::setSize (glm::vec2 _size) {
    UIComponentNode::setSize(_size);
    updateLayout();
}

void UIFlexBoxNode::placeChildren (const std::vector<UIAnchor>& anchors) {
    float currOffset = 0;
    for (int i = 0; i < children.size(); i++) {
        float primaryOffset = currOffset + glm::max(glm::dot(primaryAxisVec, anchors[i].topLeftMargin), glm::dot(primaryAxisVec, -anchors[i].bottomRightMargin));
        float secondaryOffset = glm::max(glm::dot(secondaryAxisVec, anchors[i].topLeftMargin), glm::dot(secondaryAxisVec, -anchors[i].bottomRightMargin));

        glm::vec2 offset = primaryOffset * primaryAxisVec + secondaryOffset * secondaryAxisVec;

        items.push_back({children[i].get(), offset, anchors[i].minimumSize, anchors[i].minimumSize + anchors[i].topLeftMargin + anchors[i].bottomRightMargin});

        currOffset += glm::abs(glm::dot(primaryAxisVec, anchors[i].topLeftMargin)) + glm::abs(glm::dot(primaryAxisVec, anchors[i].minimumSize))
                      + glm::abs(glm::dot(primaryAxisVec, anchors[i].bottomRightMargin));
    }
}

float UIFlexBoxNode::expandChildren (const std::vector<UIAnchor>& anchors, float remainingSize) {
    if (remainingSize <= 0) {
        return 0;
    }

    int numCanExpand = 0;
    for (int i = 0; i < children.size(); i++) {
        if (canExpand(items[i].size, primaryAxisVec, anchors[i])) {
            numCanExpand++;
        }
    }

    while (remainingSize > 0 && numCanExpand != 0) {
        float sizeIncr = remainingSize / (float) numCanExpand;
        float newOffset = 0;
        numCanExpand = 0;
        for (int i = 0; i < children.size(); i++) {
            items[i].offset += primaryAxisVec * newOffset;

            float incr = expandIfPossible(items[i], primaryAxisVec, sizeIncr, anchors[i]);
            remainingSize -= incr;
            newOffset += incr;

            if (canExpand(items[i].size, primaryAxisVec, anchors[i])) {
                numCanExpand++;
            }
        }
    }
    return remainingSize;
}

void UIFlexBoxNode::justifyChildren (float remainingSpace) {
    if (remainingSpace == 0) {
        return;
    }
    float offset;
    float betweenSpace;

    switch (justification) {
        case FlexJustify::NONE:
            return;
        case FlexJustify::CENTRE:
            offset = remainingSpace / 2;
            betweenSpace = 0.0f;
            break;
        case FlexJustify::SPACE_BETWEEN:
            offset = 0.0f;
            betweenSpace = remainingSpace / (float)(items.size() - 1);
            break;
        case FlexJustify::SPACE_AROUND:
            offset = remainingSpace / (float)(2 * items.size());
            betweenSpace = remainingSpace / (float)items.size();
            break;
        case FlexJustify::SPACE_EVENLY:
            offset = remainingSpace / (float)(items.size() + 1);
            betweenSpace = offset;
            break;
    }

    for (auto& i : items) {
        i.offset += offset * primaryAxisVec;
        offset += betweenSpace;
    }
}

void UIFlexBoxNode::setJustify (FlexJustify justify) {
    justification = justify;
    updateLayout();
}

void UIFlexBoxNode::setAlign (FlexAlign align) {
    alignment = align;
    updateLayout();
}

void UIFlexBoxNode::alignChildren (const std::vector<UIAnchor>& anchors) {
    bool isHorizontal = glm::dot({1, 0}, secondaryAxisVec) != 0;
    FloatAnchor alignAnchor;

    switch (alignment) {
        case FlexAlign::START:
            alignAnchor = FloatAnchor::FLOAT_START;
            break;
        case FlexAlign::CENTRE:
            alignAnchor = FloatAnchor::FLOAT_CENTRE;
            break;
        case FlexAlign::END:
            alignAnchor = FloatAnchor::FLOAT_END;
            break;
    }

    for (int i = 0; i < children.size(); i++) {
        auto& anchor = anchors[i];
        auto& item = items[i];

        auto floatAnchor = isHorizontal ? anchor.horizFloatAnchor : anchor.vertFloatAnchor;

        if (floatAnchor == FloatAnchor::AUTO) {
            floatAnchor = alignAnchor;
        }
        expandSecondaryAxis(item, secondaryAxisVec, glm::abs(glm::dot(size, secondaryAxisVec)), anchor);
        alignItem(item, secondaryAxisVec, glm::abs(glm::dot(size, secondaryAxisVec)), floatAnchor);
    }
}

void UIFlexBoxNode::onThemeUpdate () {
    for (auto& i : children) {
        i->applyTheme(getThemePtr());
    }

    minSize = getTheme().getProperty<glm::vec2>("size")
            .orOpt([this] () {return getTheme().getProperty<glm::vec2>("min_size");})
            .orElse({0, 0});

    maxSize = getTheme().getProperty<glm::vec2>("size")
            .orOpt([this] () {return getTheme().getProperty<glm::vec2>("max_size");})
            .orElse({-1, -1});

    Axis primAxis = getTheme().getProperty<Axis>("primary_axis").orElse(Axis::DOWN);
    Axis secondAxis = getTheme().getProperty<Axis>("secondary_axis").orElse(Axis::RIGHT);
    setAxes(primAxis, secondAxis);

    setAlign(getTheme().getProperty<FlexAlign>("align_items").orElse(FlexAlign::START));
    setJustify(getTheme().getProperty<FlexJustify>("justify_items").orElse(FlexJustify::NONE));

    updateLayout();
}

void UIFlexBoxNode::destroyChild (UIComponentNode* childNode) {
    items.erase(std::remove_if(items.begin(), items.end(), [childNode] (detail::FlexBoxItem& i) {
        return i.node == childNode;
    }), items.end());

    children.erase(std::remove_if(children.begin(), children.end(), [childNode] (std::shared_ptr<UIComponentNode>& i) {
        return i.get() == childNode;
    }), children.end());
}

bool UIFlexBoxNode::isDirty () {
    if (UIComponentNode::isDirty()) {
        return true;
    }

    if (std::any_of(children.begin(), children.end(), [] (std::shared_ptr<UIComponentNode>& i) {
        return i->isDirty();
    })) {
        markDirty();
        return true;
    } else {
        return false;
    }
}


UIFlexBoxNode::~UIFlexBoxNode () = default;
