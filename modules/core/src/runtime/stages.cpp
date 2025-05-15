#include "core/runtime.h"
#include "core/runtime/stage.h"
#include "core/runtime/system.h"
#include "util/random.h"

using namespace phenyl::core;

AbstractStage::AbstractStage (std::string name, PhenylRuntime& runtime) : m_name{std::move(name)}, m_runtime{runtime} {}

AbstractStage::~AbstractStage () = default;

void AbstractStage::addSystemUntyped (IRunnableSystem* system) {
    PHENYL_DASSERT(system);
    m_systems.emplace_back(system);
    m_updated = true;
}

void AbstractStage::run () {
    if (m_updated) {
        orderSystems();
        orderStages();
        m_updated = false;
    }

    m_runtime.world().defer();
    for (auto* i : m_orderedSystems) {
        if (i->exclusive()) {
            m_runtime.world().deferEnd();
        }

        i->run(m_runtime);

        if (i->exclusive()) {
            m_runtime.world().defer();
        }
    }
    m_runtime.world().deferEnd();

    for (auto* i : m_childStages) {
        i->run();
    }
}

void AbstractStage::addChildStage (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    m_childStages.emplace_back(stage);
    m_updated = true;
}

void AbstractStage::runBefore (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    stage->runAfter(this);
}

void AbstractStage::runAfter (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    m_prevStages.emplace(stage);
}

void AbstractStage::orderSystems () {
    m_orderedSystems.clear();

    PHENYL_DEBUG({ util::Random::Shuffle(m_systems.begin(), m_systems.end()); })

    std::unordered_set<IRunnableSystem*> visited{};
    std::unordered_set<IRunnableSystem*> visiting{};
    for (auto* i : m_systems) {
        orderSystemsRecursive(i, visited, visiting);
    }
}

void AbstractStage::orderStages () {
    std::vector<AbstractStage*> stages = m_childStages;
    m_childStages.clear();

    PHENYL_DEBUG({ util::Random::Shuffle(stages.begin(), stages.end()); })

    std::unordered_set<AbstractStage*> visited{};
    std::unordered_set<AbstractStage*> visiting{};
    for (auto* i : stages) {
        orderStagesRecursive(i, visited, visiting);
    }
}

void AbstractStage::orderSystemsRecursive (IRunnableSystem* system, std::unordered_set<IRunnableSystem*>& visited,
    std::unordered_set<IRunnableSystem*>& visiting) {
    PHENYL_ASSERT_MSG(!visiting.contains(system), "Detected cycle in system ordering!");
    if (visited.contains(system)) {
        return;
    }

    visiting.emplace(system);
    for (auto* i : system->getPrecedingSystems()) {
        orderSystemsRecursive(i, visited, visiting);
    }

    m_orderedSystems.emplace_back(system);
    visited.emplace(system);
    visiting.erase(system);
}

void AbstractStage::orderStagesRecursive (AbstractStage* stage, std::unordered_set<AbstractStage*>& visited,
    std::unordered_set<AbstractStage*>& visiting) {
    PHENYL_ASSERT_MSG(!visiting.contains(stage), "Detected cycle in stage ordering!");
    if (visited.contains(stage)) {
        return;
    }

    visiting.emplace(stage);
    for (auto* i : stage->m_prevStages) {
        orderStagesRecursive(i, visited, visiting);
    }

    m_childStages.emplace_back(stage);
    visited.emplace(stage);
    visiting.erase(stage);
}
