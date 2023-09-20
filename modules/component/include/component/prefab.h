#pragma once

#include "component.h"

namespace phenyl::component {
    class Prefab {
    private:
        std::size_t id{0};
        ComponentManager* manager{nullptr};

        void decRefCount () {
            if (manager && id) {
                manager->prefabs.decRefCount(id);
            }
        }

        Prefab (ComponentManager* manager, std::size_t id) : manager{manager}, id{id} {}
        friend class ComponentManager;
        friend class PrefabBuilder;
    public:
        class Instantiator {
        private:
            Entity entity;
            std::size_t prefabId;

            Instantiator (Entity entity, std::size_t prefabId) : entity{entity}, prefabId{prefabId} {

            }
            friend class Prefab;
        public:
            // TODO
            EntityId getId () const {
                return entity.id();
            }
            template <typename T>
            Instantiator& with (T comp) {
                entity.insert(std::move(comp));

                return *this;
            }

            Instantiator& withChild (Entity child) {
                entity.addChild(child);

                return *this;
            }

            Entity complete () {
                if (prefabId) {
                    entity.manager().prefabs.instantiate(entity, prefabId);
                }
                entity.manager().deferSignalsEnd();
                return entity;
            }
        };

        Prefab () = default;

        Prefab (const Prefab& other) : manager{other.manager}, id{other.id} {
            if (manager && id) {
                manager->prefabs.incRefCount(id);
            }
        }
        Prefab (Prefab&& other) noexcept : manager{other.manager}, id{other.id} {
            other.manager = nullptr;
            other.id = 0;
        }

        Prefab& operator= (const Prefab& other) {
            if (&other == this) {
                return *this;
            }

            decRefCount();
            manager = other.manager;
            id = other.id;

            if (manager && id) {
                manager->prefabs.incRefCount(id);
            }

            return *this;
        }
        Prefab& operator= (Prefab&& other) noexcept {
            decRefCount();
            manager = other.manager;
            id = other.id;

            other.manager = nullptr;
            other.id = 0;

            return *this;
        }

        explicit operator bool () const {
            return manager && id;
        }

        [[nodiscard]] Instantiator instantiate (Entity parent) const {
            manager->deferSignals();
            return Instantiator{parent.createChild(), id};
        }

        [[nodiscard]] Instantiator instantiate () const {
            manager->deferSignals();
            return Instantiator{manager->create(), id};
        }

        static Prefab NullPrefab (ComponentManager* manager) {
            return Prefab{manager, 0};
        }

        ~Prefab() {
            decRefCount();
        }
    };

    class PrefabBuilder {
    private:
        ComponentManager* manager;
        std::size_t prefabId;
        PrefabBuilder (ComponentManager* manager, std::size_t prefabId) : manager{manager}, prefabId{prefabId} {}

        friend class ComponentManager;
    public:
        template <typename T>
        PrefabBuilder& with (T comp) {
            auto* set = (detail::TypedComponentSet<T>*)manager->getComponent<T>();
            if (!set) {
                logging::log(LEVEL_ERROR, "Attempted to add component that doesn't exist to prefab!");
                return *this;
            }

            auto index = set->addPrefab(std::move(comp));
            manager->prefabs.addComponent(prefabId, set, index);

            return *this;
        }

        PrefabBuilder& inherits (const Prefab& inherits) {
            manager->prefabs.setInherits(prefabId, inherits.id);
            return *this;
        }

        PrefabBuilder& withChild (const Prefab& child) {
            manager->prefabs.addChild(prefabId, child.id);
            return *this;
        }

        Prefab build () {
            auto prefab = Prefab{manager, prefabId};
            prefabId = 0;

            return prefab;
        }
    };
}