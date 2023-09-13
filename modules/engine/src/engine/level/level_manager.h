#pragma once

#include <iosfwd>
#include <memory>

#include <nlohmann/json_fwd.hpp>

#include "util/map.h"
#include "common/assets/asset_manager.h"

#include "engine/level/level.h"

namespace game {
    class LevelManager : public common::AssetManager<Level> {
    private:
        util::Map<std::size_t, std::unique_ptr<Level>> levels;
        component::ComponentManager* manager;
        component::EntitySerializer* serializer;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
    protected:
        Level* load(std::istream &data, std::size_t id) override;
        [[nodiscard]] nlohmann::json dumpEntity (component::Entity entity) const;
    public:
        LevelManager (component::ComponentManager* manager, component::EntitySerializer* serializer);
        ~LevelManager() override;

        void selfRegister ();

        void dump (std::ostream& file) const;
    };
}