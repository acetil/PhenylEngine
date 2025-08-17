#include "ui/reactive/component.h"

#include "ui/reactive/ui.h"

using namespace phenyl::graphics;

UIComponentBase::~UIComponentBase () {
    for (auto* observable : m_observables) {
        observable->removeListener(*this);
    }
}

UIComponentBase::UIComponentBase (UI& ui, meta::TypeIndex compType) : m_ui{ui}, m_compType{compType} {}

void UIComponentBase::markDirty () {
    m_ui.markDirty();
}

void UIComponentBase::addObservable (IUIObservable& observable) {
    m_observables.emplace(&observable);
}
