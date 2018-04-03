#pragma once
#include <vector>
#include "out/SpriteRenderer.h"
#include "Object.h"
#include <array>

class Actor;
class Prop;
class Map;
class Tile;

struct Terrain {
	// 256
	enum class Floor { NONE = 0, DEV = 1, WATER = 11, OIL = 12, TOXIN = 13, GRASS = 16, DIRT = 17};
	enum class Fluid { NONE = 0, WATER = 3, OIL = 4, TOXIN = 5 };
	enum class Vapor { NONE = 0, FIRE = 7, POISON = 8, STEAM = 9 };
	enum class Object { NONE = 0, DEV = 10, DOOR = 14, TREE = 15 };
	Floor floor{ Floor::NONE };
	Fluid fluid{ Fluid::NONE };
	Vapor vapor{ Vapor::NONE };
	Object object{ Object::NONE };
	int health;
};

class StaticObject : public Object {
public:
	StaticObject(Map& map, Terrain& terrain, Tile& tile, fk::Texture& texture, int x, int y, int health);
	~StaticObject();
	void operator = (const StaticObject staticObject);
	void update();
	virtual void updateSprite() override;
private:
	Terrain& m_terrain;
	Tile& m_tile;
};

struct Tile {
	int floorSpriteID{ 0 };
	int fluidSpriteID{ 0 };
	int vaporSpriteID{ 0 };
	StaticObject* staticObjectPtr{ nullptr };
};

class Map {
public:
	Map();
	~Map() = default;
	void resize(int width, int height, fk::Texture& texure);
	const Terrain* getTilePtr(glm::vec2& position);
	const std::array<std::pair<Terrain*, glm::ivec2>, 8> getNeighborTilePtrs(glm::ivec2& position);
	const Terrain* getTilePtr(float x, float y);
	void render(fk::Camera& cam);
	void update(fk::Camera& cam);
	void setFloorTexture(Terrain::Floor floor, fk::Texture& texture);
	void setFluidTexture(Terrain::Fluid fluid, fk::Texture& texture);
	void setVaporTexture(Terrain::Vapor vapor, fk::Texture& texture);
	void setStaticObjectTexture(Terrain::Object staticObject, fk::Texture& texture);
	void placeFloor(Terrain::Floor type, glm::vec2& position);
	void placeFluid(Terrain::Fluid type, glm::vec2& position);
	void placeVapor(Terrain::Vapor type, glm::vec2& position);
	void placeStaticObject(Terrain::Object type, int health, glm::vec2& position);
	void placeFloor(Terrain::Floor type, float x, float y);
	void placeFluid(Terrain::Fluid type, float x, float y);
	void placeVapor(Terrain::Vapor type, float x, float y);
	void placeStaticObject(Terrain::Object type, int health, float x, float y);
	fk::SpriteBatch floorSprites{ false };
	fk::SpriteBatch fluidSprites{ true };
	fk::SpriteBatch vaporSprites{ true };
	fk::SpriteBatch staticObjectSprites{ false };
	fk::SpriteBatch dynamicObjectSprites{ true };
	fk::SpriteBatch logicSprites{ false };
	fk::World world;
	std::vector<Actor*> actorPtrs;
	std::vector<Prop*> propPtrs;
	std::vector<StaticObject*> staticObjectPtrs;
	fk::SpriteRenderer spriteRenderer;
private:
	std::vector<fk::Texture> m_terrainTextures;
	std::vector<std::vector<Tile>> m_tiles;
	std::vector<std::vector<Terrain>> m_terrain;
};