#pragma once

#include "system.h"
#include "util/meta.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace phenyl::core {
class AbstractStage {
public:
    AbstractStage (std::string name, PhenylRuntime& runtime);
    virtual ~AbstractStage ();

    void run ();

    void addChildStage (AbstractStage* stage);
    void runBefore (AbstractStage* stage);
    void runAfter (AbstractStage* stage);

    [[nodiscard]] virtual std::size_t id () const noexcept = 0;

    const std::string& name () const noexcept {
        return m_name;
    }

protected:
    std::string m_name;
    PhenylRuntime& m_runtime;
    std::vector<IRunnableSystem*> m_systems;
    std::vector<IRunnableSystem*> m_orderedSystems;

    std::vector<AbstractStage*> m_childStages;
    std::unordered_set<AbstractStage*> m_prevStages;

    bool m_updated = false;

    void addSystemUntyped (IRunnableSystem* system);
    void orderSystems ();
    void orderStages ();
    void orderSystemsRecursive (IRunnableSystem* system, std::unordered_set<IRunnableSystem*>& visited,
        std::unordered_set<IRunnableSystem*>& visiting);
    void orderStagesRecursive (AbstractStage* stage, std::unordered_set<AbstractStage*>& visited,
        std::unordered_set<AbstractStage*>& visiting);
};

template<typename S>
class Stage : public AbstractStage {
public:
    explicit Stage (std::string name, PhenylRuntime& runtime) : AbstractStage{std::move(name), runtime} {}

    [[nodiscard]] std::size_t id () const noexcept override {
        return meta::type_index<S>();
    }

    void addSystem (System<S>* system) {
        addSystemUntyped(system);
    }
};
} // namespace phenyl::core
