#pragma once

#include <iosfwd>
#include <memory>

#include <nlohmann/json_fwd.hpp>

#include "util/map.h"
#include "common/assets/asset_manager.h"
#include "runtime/iresource.h"

#include "engine/level/level.h"

namespace phenyl::game {
    // TODO: manager interface
    class LevelManager : public common::AssetManager<Level>, public runtime::IResource {
    private:
        util::Map<std::size_t, std::unique_ptr<Level>> levels;
        component::ComponentManager& manager;
        component::EntitySerializer& serializer;

        void queueUnload(std::size_t id) override;
        [[nodiscard]] const char* getFileType() const override;
    protected:
        Level* load(std::istream &data, std::size_t id) override;
        Level* load (phenyl::game::Level&& obj, std::size_t id) override;

        [[nodiscard]] nlohmann::json dumpEntity (component::Entity entity) const;
    public:
        LevelManager (component::ComponentManager& manager, component::EntitySerializer& serializer);
        ~LevelManager() override;

        [[nodiscard]] std::string_view getName() const noexcept override;

        void selfRegister ();

        void dump (std::ostream& file) const;
    };
}