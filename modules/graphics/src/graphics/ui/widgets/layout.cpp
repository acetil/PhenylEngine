#include "graphics/ui/widgets/layout.h"

#include "graphics/canvas/canvas.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

LayoutWidget::LayoutWidget (Axis axis, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier) : Widget{modifier}, axis{axis}, arrangement{arrangement}, alignment{alignment} {}

Widget* LayoutWidget::insert (std::size_t pos, std::unique_ptr<Widget> child) {
    PHENYL_ASSERT(pos <= size());
    auto* ptr = child.get();
    children.emplace(children.begin() + pos, std::move(child));
    ptr->setParent(this);
    return ptr;
}

Widget* LayoutWidget::pushBack (std::unique_ptr<Widget> child) {
    auto* ptr = child.get();
    children.emplace_back(std::move(child));
    ptr->setParent(this);
    return ptr;
}

Widget* LayoutWidget::at (std::size_t index) const {
    return children.at(index).get();
}

std::size_t LayoutWidget::size () const noexcept {
    return children.size();
}

bool LayoutWidget::empty () const noexcept {
    return children.empty();
}

void LayoutWidget::clear () {
    children.clear();
}

void LayoutWidget::update () {
    std::erase_if(children, [this] (const auto& w) {
        return widgetsToDelete.contains(w.get());
    });
    widgetsToDelete.clear();

    for (auto& i : children) {
        i->update();
    }
}

void LayoutWidget::measure (const WidgetConstraints& constraints) {
    glm::vec2 dims = glm::max(modifier().minSize,
        glm::vec2{modifier().maxWidth ? glm::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x, modifier().maxHeight ? glm::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y});

    glm::vec2 mainAxis{};
    glm::vec2 secondAxis{};
    glm::vec2 currOff{};

    switch (axis) {
        case Axis::LEFT:
        case Axis::RIGHT:
            mainAxis = {1, 0};
            secondAxis = {0, 1};
            break;
        case Axis::UP:
        case Axis::DOWN:
            mainAxis = {0, 1};
            secondAxis = {1, 0};
            break;
    }
    float secondLength = glm::dot(dims, secondAxis);
    float remainingLength = measureChildren(dims, glm::abs(mainAxis), glm::abs(secondAxis));

    float spacing = 0.0f;
    if (arrangement == LayoutArrangement::CENTER) {
        currOff += mainAxis * (remainingLength / 2);
    } else if (arrangement == LayoutArrangement::END) {
        currOff += mainAxis * remainingLength;
    } else if (arrangement == LayoutArrangement::SPACED) {
        spacing = children.size() > 1 ? remainingLength / ((float)children.size() - 1) : 0.0f;
    }

    childOff.clear();
    childOff.reserve(children.size());
    for (auto& i : children) {
        auto childDims = i->dimensions();
        float remainingSecond = std::max(0.0f, secondLength - glm::dot(childDims, secondAxis) - i->modifier().padding * 2 - glm::dot(secondAxis, i->modifier().offset));
        switch (alignment) {
            case LayoutAlignment::START:
                childOff.emplace_back(currOff + secondAxis * i->modifier().padding + i->modifier().offset);
                break;
            case LayoutAlignment::END:
                childOff.emplace_back(currOff + secondAxis * (remainingSecond + i->modifier().padding) + i->modifier().offset);
                break;
            case LayoutAlignment::CENTER:
                childOff.emplace_back(currOff + secondAxis * (remainingSecond / 2 + i->modifier().padding) + i->modifier().offset);
                break;
        }

        currOff += mainAxis * (glm::dot(mainAxis, childDims + i->modifier().offset) + i->modifier().padding * 2 + spacing);
    }

    if (axis == Axis::UP || axis == Axis::LEFT) {
        // Flip offsets
        float totalLength = glm::dot(mainAxis, dims);
        for (std::size_t i = 0; i < children.size(); i++) {
            childOff[i] = secondAxis * glm::dot(childOff[i], secondAxis) + mainAxis * (totalLength - glm::dot(childOff[i] + children[i]->dimensions(), mainAxis));
        }
    }

    setDimensions(dims);
}

void LayoutWidget::render (Canvas& canvas) {
    for (std::size_t i = 0; i < size(); i++) {
        canvas.pushOffset(childOff[i]);
        children[i]->render(canvas);
        canvas.popOffset();
    }
}

void LayoutWidget::queueChildDestroy (Widget* child) {
    widgetsToDelete.emplace(child);
}

float LayoutWidget::measureChildren (glm::vec2 dims, glm::vec2 mainAxis, glm::vec2 secondAxis) {
    float remainingLength = glm::dot(mainAxis, dims);
    glm::vec2 secondDim = secondAxis * glm::dot(secondAxis, dims);
    float totalWeight = 0.0f;
    for (auto& i : children) {
        if (i->modifier().weight > 0.0f) {
            totalWeight += i->modifier().weight;
        } else {
            i->measure(WidgetConstraints {
                .maxSize = (remainingLength  - 2 * i->modifier().padding) * mainAxis + (secondDim - 2 * i->modifier().padding * secondAxis) - i->modifier().offset
            });

            remainingLength = std::max(0.0f, remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding - glm::dot(mainAxis, i->modifier().offset));
        }
    }

    if (totalWeight == 0.0f) {
        return std::max(0.0f, remainingLength);
    }

    for (auto& i : children) {
        if (i->modifier().weight > 0.0f) {
            i->measure(WidgetConstraints{
                .maxSize = (remainingLength * i->modifier().weight / totalWeight - 2 * i->modifier().padding) * mainAxis + (secondDim - 2 * i->modifier().padding * secondAxis)  - i->modifier().offset
            });
            totalWeight -= i->modifier().weight;
            remainingLength = std::max(0.0f, remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding - glm::dot(mainAxis, i->modifier().offset));
        }
    }

    return std::max(0.0f, remainingLength);
}

ColumnWidget::ColumnWidget (ColumnDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier) : LayoutWidget{dir == ColumnDirection::DOWN ? Axis::DOWN : Axis::UP, arrangement, alignment, modifier} {}

RowWidget::RowWidget (RowDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier) : LayoutWidget{dir == RowDirection::RIGHT ? Axis::RIGHT : Axis::LEFT, arrangement, alignment, modifier} {}
