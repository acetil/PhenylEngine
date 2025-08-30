#include "ui/reactive/component.h"

#include "ui/reactive/ui.h"

using namespace phenyl::graphics;

UIComponentBase::~UIComponentBase () {
    for (auto* observable : m_observables) {
        observable->removeListener(*this);
    }
    m_ui.onComponentDestroy(id());
}

UIComponentBase::UIComponentBase (UI& ui, meta::TypeIndex compType) :
    m_ui{ui},
    m_compType{compType},
    m_id{ui.makeId()} {}

void UIComponentBase::markDirty () {
    m_ui.markDirty(m_id);
}

void UIComponentBase::addObservable (IUIObservable& observable) {
    m_observables.emplace(&observable);
}
