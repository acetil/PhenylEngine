#include <string>
#include "game/tile/tile.h"
#include "util/smart_help.h"
#ifndef MAP_H
#define MAP_H
namespace game {
    struct MapEntity {
        std::string entityType;
        float x = 0.0f;
        float y = 0.0f;
        float rotation = 0.0f;
        std::string extraOpts;
        MapEntity () = default;
        MapEntity(std::string _type, float _x, float _y, float _rot, std::string _opts) : entityType(std::move(_type)),
                x(_x), y(_y), rotation(_rot), extraOpts(std::move(_opts)) {};


    };
    class Map : public util::SmartHelper<Map> {
        protected:
        int width;
        int height;
        Tile** tiles;
        //graphics::StaticData* graphicsData;
        virtual Tile** getTiles ();
        graphics::TextureAtlas atlas;
        std::vector<MapEntity> entities;
        public:
        Map (int xSize, int ySize);
        ~Map();
        virtual Tile* getTile (int x, int y);
        virtual void setTiles (Tile** tiles);
        virtual int getWidth ();
        virtual int getHeight ();
        //virtual void initGraphicsData (graphics::Graphics* graphics, std::string shader);
        //virtual void unloadGraphicsData (graphics::Graphics* graphics);

        //virtual int getNumTileVertices (); // TODO: move to models
        //virtual float* getTileVertices ();
        //virtual float* getTileUvs ();
        virtual std::vector<std::pair<graphics::AbsolutePosition, graphics::FixedModel>> getModels ();
        virtual void setAtlas (graphics::TextureAtlas atlas);

        virtual void setEntities (std::vector<MapEntity>& _entities) {
            entities = std::move(_entities);
        }

        virtual const std::vector<MapEntity>& getEntities () {
            return entities;
        }
        virtual void writeMapJson (const std::string& path);
    };
}
#endif