#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "util/meta.h"

#include "system.h"

namespace phenyl::runtime {
    class PhenylRuntime;

    struct PostInit {};
    struct FrameBegin {};
    struct Update {};
    struct Render {};
    struct FixedUpdate {};
    struct PhysicsUpdate {};

    class AbstractStage {
    protected:
        std::string stageName;
        PhenylRuntime& runtime;
        std::vector<IRunnableSystem*> systems;
        std::vector<IRunnableSystem*> orderedSystems;

        std::vector<AbstractStage*> childStages;
        std::unordered_set<AbstractStage*> prevStages;

        bool updated = false;
        void addSystemUntyped (IRunnableSystem* system);
        void orderSystems ();
        void orderStages ();
        void orderSystemsRecursive (IRunnableSystem* system, std::unordered_set<IRunnableSystem*>& visited, std::unordered_set<IRunnableSystem*>& visiting);
        void orderStagesRecursive (AbstractStage* stage, std::unordered_set<AbstractStage*>& visited, std::unordered_set<AbstractStage*>& visiting);
    public:
        AbstractStage (std::string name, PhenylRuntime& runtime);
        virtual ~AbstractStage ();

        void run ();

        void addChildStage (AbstractStage* stage);
        void runBefore (AbstractStage* stage);
        void runAfter (AbstractStage* stage);

        [[nodiscard]] virtual std::size_t id () const noexcept = 0;

        const std::string& name () const noexcept {
            return stageName;
        }
    };

    template <typename S>
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
}