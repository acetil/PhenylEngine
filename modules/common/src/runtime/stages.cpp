#include "util/random.h"

#include "core/runtime/stage.h"

#include "core/runtime.h"
#include "core/runtime/system.h"

using namespace phenyl::core;

AbstractStage::AbstractStage (std::string name, PhenylRuntime& runtime) : stageName{std::move(name)}, runtime{runtime} {}
AbstractStage::~AbstractStage () = default;

void AbstractStage::addSystemUntyped (IRunnableSystem* system) {
    PHENYL_DASSERT(system);
    systems.emplace_back(system);
    updated = true;
}

void AbstractStage::run () {
    if (updated) {
        orderSystems();
        orderStages();
        updated = false;
    }

    runtime.world().defer();
    for (auto* i : orderedSystems) {
        if (i->exclusive()) {
            runtime.world().deferEnd();
        }

        i->run(runtime);

        if (i->exclusive()) {
            runtime.world().defer();
        }
    }
    runtime.world().deferEnd();

    for (auto* i : childStages) {
        i->run();
    }
}

void AbstractStage::addChildStage (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    childStages.emplace_back(stage);
    updated = true;
}

void AbstractStage::runBefore (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    stage->runAfter(this);
}

void AbstractStage::runAfter (AbstractStage* stage) {
    PHENYL_DASSERT(stage);
    prevStages.emplace(stage);
}

void AbstractStage::orderSystems () {
    orderedSystems.clear();

    PHENYL_DEBUG({
        util::Random::Shuffle(systems.begin(), systems.end());
    })

    std::unordered_set<IRunnableSystem*> visited{};
    std::unordered_set<IRunnableSystem*> visiting{};
    for (auto* i : systems) {
        orderSystemsRecursive(i, visited, visiting);
    }
}

void AbstractStage::orderStages () {
    std::vector<AbstractStage*> stages = childStages;
    childStages.clear();

    PHENYL_DEBUG({
        util::Random::Shuffle(stages.begin(), stages.end());
    })

    std::unordered_set<AbstractStage*> visited{};
    std::unordered_set<AbstractStage*> visiting{};
    for (auto* i : stages) {
        orderStagesRecursive(i, visited, visiting);
    }
}

void AbstractStage::orderSystemsRecursive (IRunnableSystem* system, std::unordered_set<IRunnableSystem*>& visited, std::unordered_set<IRunnableSystem*>& visiting) {
    PHENYL_ASSERT_MSG(!visiting.contains(system), "Detected cycle in system ordering!");
    if (visited.contains(system)) {
        return;
    }

    visiting.emplace(system);
    for (auto* i : system->getPrecedingSystems()) {
        orderSystemsRecursive(i, visited, visiting);
    }

    orderedSystems.emplace_back(system);
    visited.emplace(system);
    visiting.erase(system);
}

void AbstractStage::orderStagesRecursive (AbstractStage* stage, std::unordered_set<AbstractStage*>& visited, std::unordered_set<AbstractStage*>& visiting) {
    PHENYL_ASSERT_MSG(!visiting.contains(stage), "Detected cycle in stage ordering!");
    if (visited.contains(stage)) {
        return;
    }

    visiting.emplace(stage);
    for (auto* i : stage->prevStages) {
        orderStagesRecursive(i, visited, visiting);
    }

    childStages.emplace_back(stage);
    visited.emplace(stage);
    visiting.erase(stage);
}
