#pragma once

#include <string>
#include "engine/tile/tile.h"
#include "util/smart_help.h"
#include "util/data.h"
#include "graphics/maths_headers.h"

namespace graphics {
    struct Transform2D;
    struct Model2D;
}

namespace game {
    struct MapEntity {
        util::DataValue data;
        MapEntity () = default;
        MapEntity(util::DataValue _data) : data(std::move(_data)) {};


    };
    class Map : public util::SmartHelper<Map> {
        protected:
        int width;
        int height;
        Tile** tiles;
        virtual Tile** getTiles ();
        graphics::TextureAtlas* atlas;
        std::vector<MapEntity> entities;
        public:
        Map (int xSize, int ySize);
        ~Map();
        virtual Tile* getTile (int x, int y);
        virtual void setTiles (Tile** tiles);
        virtual int getWidth ();
        virtual int getHeight ();

        virtual std::vector<std::tuple<glm::vec2, graphics::Transform2D, graphics::Model2D>> getModels ();
        virtual void setAtlas (graphics::TextureAtlas& atlas);

        virtual void setEntities (std::vector<MapEntity>& _entities) {
            entities = std::move(_entities);
        }

        virtual const std::vector<MapEntity>& getEntities () {
            return entities;
        }
        virtual void writeMapJson (const std::string& path, util::DataValue entities = util::DataValue());
    };
}
