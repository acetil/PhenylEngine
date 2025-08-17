#pragma once

#include "atom.h"
#include "state.h"
#include "util/meta.h"
#include "util/type_index.h"

#include <unordered_set>

namespace phenyl::graphics {
class UI;

class UIComponentBase : protected IUIDirtyable {
public:
    ~UIComponentBase () override;

    meta::TypeIndex type () const noexcept {
        return m_compType;
    }

    virtual void render (UI& ui) = 0;

protected:
    UIComponentBase (UI& ui, meta::TypeIndex compType);

    template <typename T>
    UIState<T> makeState (T&& defaultVal) {
        return UIState<T>{*this, std::forward<T>(defaultVal)};
    }

    template <typename T>
    void useAtom (const UIAtom<T>& atom) {
        atom.addUsing(*this);
    }

    void markDirty () override;

private:
    UI& m_ui;
    meta::TypeIndex m_compType;
    std::vector<UIComponentBase*> m_children;

    std::unordered_set<IUIObservable*> m_observables;

    void addObservable (IUIObservable& observable) override;
};

template <typename CompType, typename Props>
class UIComponent : public UIComponentBase {
public:
    UIComponent (UI& ui, Props&& props) :
        UIComponentBase{ui, meta::TypeIndex::Get<CompType>()},
        m_props{std::move(props)} {}

    const Props& props () const noexcept {
        return m_props;
    }

    void setProps (Props&& props) {
        m_props = std::move(props);
    }

private:
    Props m_props;
};

template <typename CompType>
class UIComponent<CompType, void> : public UIComponentBase {
public:
    UIComponent (UI& ui) : UIComponentBase{ui, meta::TypeIndex::Get<CompType>()} {}
};

template <typename T> concept UIComponentTypeVoidProps =
    std::derived_from<T, UIComponent<T, void>> && std::constructible_from<T, UI&>;

template <typename T, typename Props> concept UIComponentType =
    (std::derived_from<T, UIComponent<T, Props>> && std::constructible_from<T, UI&, Props&&>);
} // namespace phenyl::graphics
