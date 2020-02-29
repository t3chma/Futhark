#pragma once
#include <vector>
#include "base/Utility.h"
#include "../Image.h"
#include "Structure.h"
#include "Building.h"
#include "Climate.h"
#include "../actors/Person.h"
#include "../actors/Vehicle.h"
#include <set>


class Tile : public Image {
  public:
	Tile(fk::SpriteBatch& sb, fk::TextureCache& t, double x, double y);
	~Tile();
	Climate climate;
	std::vector<Structure> structs;
	void updateColor() {}
	glm::ivec2 gridPos;
	struct Units {
		Building* buildingPtr{ nullptr };
		std::vector<Person*> peoplePtrs;
		Vehicle* vehiclePtr{ nullptr };
	} units;
	void draw() override;
	void interact();
};

class Map {
  public:
	Map(fk::SpriteBatch& sb, fk::TextureCache& t);
	~Map();
	void generate(int radius);
	Tile* getTileAt(glm::vec2 mousePos);
	std::vector<glm::ivec2> collectString(glm::ivec2 start, int length, char direction);
	std::vector<glm::ivec2> getSurroundingRing(glm::ivec2 center, int radius);
	std::vector<Tile*> getTiles(glm::ivec2 center, const std::vector<glm::ivec2>& set);
	std::list<Tile> tiles;
	bool day{ true };
	bool edit{ true };
  private:
	double mapDiameter{ 0 };
	double mapRadius{ 0 };
	double stretch{ 0.865 };
	fk::SpriteBatch& spriteBatch;
	fk::TextureCache& textures;
	std::vector<std::vector<Tile*>> tileGrid;
};