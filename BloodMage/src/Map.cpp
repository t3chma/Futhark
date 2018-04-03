#include "Map.h"
#include "props/Prop.h"
#include "actors/Actor.h"
#include <thread>


StaticObject::StaticObject(
	Map& map,
	Terrain& terrain,
	Tile& tile,
	fk::Texture& texture,
	int x,
	int y ,
	int health
)
	: Object(map.staticObjectSprites, map.world, b2_staticBody, x, y),
	m_terrain(terrain),
	m_tile(tile)
{
	if (tile.staticObjectPtr) {
		tile.staticObjectPtr->health = -1;
	}
	tile.staticObjectPtr = this;
	if (texture.id) {
		spriteIDs.push_back(map.staticObjectSprites.makeSprite(texture));
		spriteBatch[spriteIDs.back()].setPosition(x, y);
		spriteBatch[spriteIDs.back()].setDimensions(1.01, 1.01);
		this->health = health;
	} else {
		this->health = -1;
	}
	m_terrain.object = Terrain::Object::DEV;
	b2FixtureDef fixtureDef1;
	b2PolygonShape shape1;
	fixtureDef1.shape = &shape1;
	shape1.SetAsBox(0.49, 0.49);
	fixtureDef1.userData = nullptr;
	fixtureDef1.friction = 0.3f;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "object";
}
StaticObject::~StaticObject() {
	for (auto&& id : spriteIDs) { spriteBatch.destroySprite(id); }
}

void StaticObject::operator=(const StaticObject staticObject) {
	m_terrain = staticObject.m_terrain;
	m_tile = staticObject.m_tile;
}
void StaticObject::update() {
	m_terrain.health = health;
	if (health < 1) { despawn = true; }
}
void StaticObject::updateSprite() {

}

Map::Map() {
	m_terrainTextures.resize(256);
}
void Map::resize(int width, int height, fk::Texture& texture) {
	setFloorTexture(Terrain::Floor::DEV, texture);
	m_terrain.resize(width);
	m_tiles.resize(width);
	for (auto&& col : m_terrain) { col.resize(height); }
	for (auto&& col : m_tiles) { col.resize(height); }
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int devID = floorSprites.makeSprite(texture);
			floorSprites[devID].setPosition(x, y);
			m_tiles[x][y].floorSpriteID = devID;
			m_terrain[x][y].floor = Terrain::Floor::DEV;
		}
	}
}
const Terrain* Map::getTilePtr(glm::vec2& position) {
	return getTilePtr(position.x, position.y);
}
const std::array<std::pair<Terrain*, glm::ivec2>, 8> Map::getNeighborTilePtrs(glm::ivec2& position) {
	std::array<std::pair<Terrain*, glm::ivec2>, 8> ret;
	int xi = position.x - 1;
	int yi = position.y - 1;
	int index = 0;
	for (int x = xi; x < xi + 3; ++x) {
		for (int y = yi; y < yi + 3; ++y) {
			if (x == position.x && y == position.y) {
				continue;
			} else if (x < 0 || x >= m_terrain.size() || y < 0 || y >= m_terrain[x].size()) {
				ret[index] = std::make_pair(nullptr, glm::ivec2(0));
			} else {
				ret[index] = std::make_pair(&m_terrain[x][y], glm::ivec2(x,y));
			}
			++index;
		}
	}
	return ret;
}
const Terrain* Map::getTilePtr(float x, float y) {
	int xi = round(x);
	int yi = round(y);
	if (xi < 0 || xi >= m_terrain.size() || yi < 0 || yi >= m_terrain[xi].size()) { return nullptr; }
	return &m_terrain[xi][yi];
}
void Map::render(fk::Camera& cam) {
	spriteRenderer.render(floorSprites, cam.getTransScaledMatrix());
	spriteRenderer.render(fluidSprites, cam.getTransScaledMatrix());
	spriteRenderer.render(staticObjectSprites, cam.getTransScaledMatrix());
	spriteRenderer.render(dynamicObjectSprites, cam.getTransScaledMatrix());
	spriteRenderer.render(vaporSprites, cam.getTransScaledMatrix());
	spriteRenderer.render(logicSprites, cam.getTransScaledMatrix());
}
void Map::update(fk::Camera& cam) {
	for (int i = 0; i < staticObjectPtrs.size(); ++i) {
		staticObjectPtrs[i]->update();
		if (staticObjectPtrs[i]->despawn) {
			delete staticObjectPtrs[i];
			staticObjectPtrs[i--] = staticObjectPtrs.back();
			staticObjectPtrs.pop_back();
		}
	}
	for (int i = 0; i < propPtrs.size(); ++i) {
		propPtrs[i]->update(&cam);
		if (propPtrs[i]->despawn) {
			delete propPtrs[i];
			propPtrs[i--] = propPtrs.back();
			propPtrs.pop_back();
		}
	}
	for (int i = 0; i < actorPtrs.size(); ++i) {
		actorPtrs[i]->think(actorPtrs, &cam);
		if (actorPtrs[i]->despawn) {
			delete actorPtrs[i];
			actorPtrs[i--] = actorPtrs.back();
			actorPtrs.pop_back();
		}
	}
	for (auto&& propPtr : propPtrs) { propPtr->updateBody(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateBody(); }
	world.update(1.0f / 60.0f, 4, 2);
	for (auto&& propPtr : propPtrs) { propPtr->updateSprite(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateSprite(); }
}
void Map::setFloorTexture(Terrain::Floor floor, fk::Texture& texture) {
	m_terrainTextures[(int)floor] = texture;
	///
}
void Map::setFluidTexture(Terrain::Fluid fluid, fk::Texture& texture) {
	m_terrainTextures[(int)fluid] = texture;
	///
}
void Map::setVaporTexture(Terrain::Vapor vapor, fk::Texture& texture) {
	m_terrainTextures[(int)vapor] = texture;
	///
}
void Map::setStaticObjectTexture(Terrain::Object Object, fk::Texture& texture) {
	m_terrainTextures[(int)Object] = texture;
	///
}
void Map::placeFloor(Terrain::Floor type, glm::vec2& position) {
	placeFloor(type, position.x, position.y);
}
void Map::placeFluid(Terrain::Fluid type, glm::vec2& position) {
	placeFluid(type, position.x, position.y);
}
void Map::placeVapor(Terrain::Vapor type, glm::vec2& position) {
	placeVapor(type, position.x, position.y);
}
void Map::placeStaticObject(Terrain::Object type, int health, glm::vec2& position) {
	placeStaticObject(type, health, position.x, position.y);
}
void Map::placeFloor(Terrain::Floor type, float x, float y) {
	int xi = round(x);
	int yi = round(y);
	if (xi < 0 || xi >= m_terrain.size() || yi < 0 || yi >= m_terrain[xi].size()) { return; }
	if (m_tiles[xi][yi].floorSpriteID) {
		floorSprites.destroySprite(m_tiles[xi][yi].floorSpriteID);
	}
	if (m_terrainTextures[(int)type].id) {
		int id = floorSprites.makeSprite(m_terrainTextures[(int)type]);
		floorSprites[id].setPosition(xi, yi);
		m_tiles[xi][yi].floorSpriteID = id;
	}
	m_terrain[xi][yi].floor = type;
}
void Map::placeFluid(Terrain::Fluid type, float x, float y) {
	int xi = round(x);
	int yi = round(y);
	if (xi < 0 || xi >= m_terrain.size() || yi < 0 || yi >= m_terrain[xi].size()) { return; }
	if (m_tiles[xi][yi].fluidSpriteID) {
		fluidSprites.destroySprite(m_tiles[xi][yi].fluidSpriteID);
	}
	if (m_terrainTextures[(int)type].id) {
		int id = fluidSprites.makeSprite(m_terrainTextures[(int)type]);
		fluidSprites[id].setPosition(xi, yi);
		fluidSprites[id].setColor(255, 255, 255, 100);
		m_tiles[xi][yi].fluidSpriteID = id;
	}
	m_terrain[xi][yi].fluid = type;
}
void Map::placeVapor(Terrain::Vapor type, float x, float y) {
	int xi = round(x);
	int yi = round(y);
	if (xi < 0 || xi >= m_terrain.size() || yi < 0 || yi >= m_terrain[xi].size()) { return; }
	if (m_tiles[xi][yi].vaporSpriteID) {
		vaporSprites.destroySprite(m_tiles[xi][yi].vaporSpriteID);
	}
	if (m_terrainTextures[(int)type].id) {
		int id = vaporSprites.makeSprite(m_terrainTextures[(int)type]);
		vaporSprites[id].setPosition(xi, yi);
		vaporSprites[id].setColor(255, 255, 255, 100);
		m_tiles[xi][yi].vaporSpriteID = id;
	}
	m_terrain[xi][yi].vapor = type;
}
void Map::placeStaticObject(Terrain::Object type, int health, float x, float y) {
	int xi = round(x);
	int yi = round(y);
	if (xi < 0 || xi >= m_terrain.size() || yi < 0 || yi >= m_terrain[xi].size()) { return; }
	auto& tile = m_tiles[xi][yi];
	staticObjectPtrs.push_back(
		new StaticObject(*this, m_terrain[xi][yi], tile, m_terrainTextures[(int)type], xi, yi, health)
	);
}