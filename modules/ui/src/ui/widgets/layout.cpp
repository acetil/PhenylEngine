#include "ui/widgets/layout.h"

#include "graphics/canvas/canvas.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

LayoutWidget::LayoutWidget (Axis axis, LayoutArrangement arrangement, LayoutAlignment alignment,
    const Modifier& modifier) :
    Widget{modifier},
    m_axis{axis},
    m_arrangement{arrangement},
    m_alignment{alignment} {}

Widget* LayoutWidget::insert (std::size_t pos, std::unique_ptr<Widget> child) {
    PHENYL_ASSERT(pos <= size());
    auto* ptr = child.get();
    m_children.emplace(m_children.begin() + pos, std::move(child));
    m_childOff.emplace(m_childOff.begin() + pos, std::numeric_limits<float>::signaling_NaN(),
        std::numeric_limits<float>::signaling_NaN());
    ptr->setParent(this);
    return ptr;
}

Widget* LayoutWidget::pushBack (std::unique_ptr<Widget> child) {
    auto* ptr = child.get();
    m_children.emplace_back(std::move(child));
    m_childOff.emplace_back(std::numeric_limits<float>::signaling_NaN(), std::numeric_limits<float>::signaling_NaN());
    ptr->setParent(this);
    return ptr;
}

Widget* LayoutWidget::at (std::size_t index) const {
    return m_children.at(index).get();
}

std::size_t LayoutWidget::size () const noexcept {
    return m_children.size();
}

bool LayoutWidget::empty () const noexcept {
    return m_children.empty();
}

void LayoutWidget::clear () {
    m_children.clear();
}

Widget* LayoutWidget::pick (glm::vec2 pointer) noexcept {
    if (pointer.x < 0.0f || pointer.x >= dimensions().x || pointer.y < 0.0f || pointer.y >= dimensions().y) {
        return nullptr;
    }

    for (std::size_t i = 0; i < size(); i++) {
        if (std::isnan(m_childOff[i].x) || std::isnan(m_childOff[i].y)) {
            continue;
        }

        if (auto* ptr = m_children[i]->pick(pointer - m_childOff[i])) {
            return ptr;
        }
    }

    return this;
}

void LayoutWidget::setOffset (glm::vec2 newOffset) {
    Widget::setOffset(newOffset);

    if (m_childOff.empty()) {
        return;
    }

    assert(m_childOff.size() == m_children.size());
    for (std::size_t i = 0; i < m_children.size(); i++) {
        m_children[i]->setOffset(offset() + m_childOff[i]);
    }
}

void LayoutWidget::update () {
    std::erase_if(m_children, [this] (const auto& w) { return m_widgetsToDelete.contains(w.get()); });
    m_widgetsToDelete.clear();

    for (auto& i : m_children) {
        i->update();
    }
}

void LayoutWidget::measure (const WidgetConstraints& constraints) {
    glm::vec2 dims = glm::max(modifier().minSize,
        glm::vec2{modifier().maxWidth ? glm::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x,
          modifier().maxHeight ? glm::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y});

    glm::vec2 mainAxis{};
    glm::vec2 secondAxis{};
    glm::vec2 currOff{};

    switch (m_axis) {
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
    if (m_arrangement == LayoutArrangement::CENTER) {
        currOff += mainAxis * (remainingLength / 2);
    } else if (m_arrangement == LayoutArrangement::END) {
        currOff += mainAxis * remainingLength;
    } else if (m_arrangement == LayoutArrangement::SPACED) {
        spacing = m_children.size() > 1 ? remainingLength / ((float) m_children.size() - 1) : 0.0f;
    }

    m_childOff.clear();
    for (auto& i : m_children) {
        auto childDims = i->dimensions();
        float remainingSecond = std::max(0.0f,
            secondLength - glm::dot(childDims, secondAxis) - i->modifier().padding * 2 -
                glm::dot(secondAxis, i->modifier().offset));
        switch (m_alignment) {
        case LayoutAlignment::START:
            m_childOff.emplace_back(currOff + secondAxis * i->modifier().padding + i->modifier().offset);
            break;
        case LayoutAlignment::END:
            m_childOff.emplace_back(
                currOff + secondAxis * (remainingSecond + i->modifier().padding) + i->modifier().offset);
            break;
        case LayoutAlignment::CENTER:
            m_childOff.emplace_back(
                currOff + secondAxis * (remainingSecond / 2 + i->modifier().padding) + i->modifier().offset);
            break;
        }
        i->setOffset(offset() + m_childOff.back());

        currOff +=
            mainAxis * (glm::dot(mainAxis, childDims + i->modifier().offset) + i->modifier().padding * 2 + spacing);
    }

    if (m_axis == Axis::UP || m_axis == Axis::LEFT) {
        // Flip offsets
        float totalLength = glm::dot(mainAxis, dims);
        for (std::size_t i = 0; i < m_children.size(); i++) {
            m_childOff[i] = secondAxis * glm::dot(m_childOff[i], secondAxis) +
                mainAxis * (totalLength - glm::dot(m_childOff[i] + m_children[i]->dimensions(), mainAxis));
        }
    }

    setDimensions(dims);
}

void LayoutWidget::render (Canvas& canvas) {
    for (std::size_t i = 0; i < size(); i++) {
        canvas.pushOffset(m_childOff[i]);
        m_children[i]->render(canvas);
        canvas.popOffset();
    }
}

void LayoutWidget::queueChildDestroy (Widget* child) {
    m_widgetsToDelete.emplace(child);
}

bool LayoutWidget::pointerUpdate (glm::vec2 pointer) {
    assert(m_children.size() == m_childOff.size());

    auto it = m_children.rbegin();
    auto offIt = m_childOff.rbegin();
    while (it != m_children.rend()) {
        bool childResult = (*it)->pointerUpdate(pointer - *offIt);
        ++it;
        ++offIt;
        if (childResult) {
            break;
        }
    }

    for (; it != m_children.rend(); ++it) {
        (*it)->pointerLeave();
        ++offIt;
    }

    return Widget::pointerUpdate(pointer);
}

void LayoutWidget::pointerLeave () {
    for (auto& i : m_children) {
        i->pointerLeave();
    }
    Widget::pointerLeave();
}

float LayoutWidget::measureChildren (glm::vec2 dims, glm::vec2 mainAxis, glm::vec2 secondAxis) {
    float remainingLength = glm::dot(mainAxis, dims);
    glm::vec2 secondDim = secondAxis * glm::dot(secondAxis, dims);
    float totalWeight = 0.0f;
    for (auto& i : m_children) {
        if (i->modifier().weight > 0.0f) {
            totalWeight += i->modifier().weight;
        } else {
            i->measure(WidgetConstraints{.maxSize = (remainingLength - 2 * i->modifier().padding) * mainAxis +
                  (secondDim - 2 * i->modifier().padding * secondAxis) - i->modifier().offset});

            remainingLength = std::max(0.0f,
                remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding -
                    glm::dot(mainAxis, i->modifier().offset));
        }
    }

    if (totalWeight == 0.0f) {
        return std::max(0.0f, remainingLength);
    }

    for (auto& i : m_children) {
        if (i->modifier().weight > 0.0f) {
            i->measure(WidgetConstraints{
              .maxSize = (remainingLength * i->modifier().weight / totalWeight - 2 * i->modifier().padding) * mainAxis +
                  (secondDim - 2 * i->modifier().padding * secondAxis) - i->modifier().offset});
            totalWeight -= i->modifier().weight;
            remainingLength = std::max(0.0f,
                remainingLength - glm::dot(mainAxis, i->dimensions()) - 2 * i->modifier().padding -
                    glm::dot(mainAxis, i->modifier().offset));
        }
    }

    return std::max(0.0f, remainingLength);
}

ColumnWidget::ColumnWidget (ColumnDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment,
    const Modifier& modifier) :
    LayoutWidget{dir == ColumnDirection::DOWN ? Axis::DOWN : Axis::UP, arrangement, alignment, modifier} {}

RowWidget::RowWidget (RowDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment,
    const Modifier& modifier) :
    LayoutWidget{dir == RowDirection::RIGHT ? Axis::RIGHT : Axis::LEFT, arrangement, alignment, modifier} {}
