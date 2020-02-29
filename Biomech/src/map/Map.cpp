#include "Map.h"


Map::Map(fk::SpriteBatch& sb, fk::TextureCache& t) : spriteBatch(sb), textures(t) {

}

Map::~Map() {

}

void Map::generate(int radius) {
	mapRadius = radius;
	mapDiameter = radius * 2 - 1;
	int side = mapDiameter - radius + 2;
	int generate = side;
	bool expanding = true;
	float x = -side * stretch + stretch;
	float y = -(float)side / 2.0l + 0.5l;
	while (radius) {
		tileGrid.emplace_back();
		for (int i = 0; i < generate; ++i) {
			tiles.emplace_back(spriteBatch, textures, tileGrid.size() - 1, i);
			tileGrid.back().push_back(&tiles.back());
			tiles.back().sprites.back().setPosition(x, y + (float)i);
		}
		x += stretch;
		if (expanding){
			y -= 0.5;
			if(generate++ > mapDiameter) { expanding = false; }
		} else {
			y += 0.5;
			if (--generate < side) { break; }
		}
	}
}

Tile::Tile(fk::SpriteBatch& sb, fk::TextureCache& t, double x, double y) : Image(sb), gridPos(x,y), climate(t, sb) {
	addSprite(t.get("Tile.png", 1));
	sprites.back().setDimensions(1.135,1.135);
}

Tile::~Tile() {

}

void Tile::draw() {

}

Tile* Map::getTileAt(glm::vec2 mousePos) {
	int x = floor(mousePos.x / stretch + mapDiameter / 2.0l) + 1.0l;
	int y = floor(mousePos.y + mapDiameter / 2.0l + abs(mousePos.x) / -stretch / 2.0l) + 1.0l;
	if (x < 0 || x >= tileGrid.size() || y < 0 || y >= tileGrid[x].size()) {
		return nullptr;
	} else {
		glm::vec2 dist = glm::vec2(mousePos.x, mousePos.y) - tileGrid[x][y]->sprites.front().getPosition();
		if (abs(glm::length(dist)) > 0.5) { return nullptr; }
		else { return tileGrid[x][y]; }
	}
}

std::vector<glm::ivec2> Map::collectString(glm::ivec2 start, int length, char direction) {
	std::vector<glm::ivec2> string;
	for (int i = 0; i < length; ++i) {
		switch (direction % 6) {
			case 0: string.push_back(start + glm::ivec2(i, 0)); break;
			case 1: string.push_back(start + glm::ivec2(i, i)); break;
			case 2: string.push_back(start + glm::ivec2(0, i)); break;
			case 3: string.push_back(start + glm::ivec2(-i, 0)); break;
			case 4: string.push_back(start + glm::ivec2(-i, -i)); break;
			case 5: string.push_back(start + glm::ivec2(0, -i)); break;
			default: break;
		}
	}
	return string;
}

std::vector<glm::ivec2> Map::getSurroundingRing(glm::ivec2 center, int radius) {
	std::vector<glm::ivec2> ret;
	glm::ivec2 c;
	for (int i = 0; i < 6; ++i) {
		c = center;
		switch (i) {
			case 0: c += glm::ivec2(radius, 0); break;
			case 1: c += glm::ivec2(radius, radius); break;
			case 2: c += glm::ivec2(0, radius); break;
			case 3: c += glm::ivec2(-radius, 0); break;
			case 4: c += glm::ivec2(-radius, -radius); break;
			case 5: c += glm::ivec2(0, -radius); break;
			default: break;
		}
		for (auto&& s : collectString(c, radius, i + 2)) { ret.push_back(s); }
	}
	return ret;
}

std::vector<Tile*> Map::getTiles(glm::ivec2 center, const std::vector<glm::ivec2>& set) {
	std::vector<Tile*> ret;
	float ny;
	for (auto& pos : set) {
		if (center.x > mapRadius) {
			if (pos.x > mapRadius) { ny = pos.y - (pos.x - center.x); }
			else { ny = pos.y + center.x - mapRadius; }
		} else {
			if (pos.x > mapRadius) { ny = pos.y - (center.x - mapRadius) - (pos.x - center.x); }
			else { ny = pos.y; }
		}
		if (!(pos.x < 0 || pos.x >= tileGrid.size() || ny < 0 || ny >= tileGrid[pos.x].size())) {
			ret.push_back(tileGrid[pos.x][ny]);
		}
	}
	return ret;
}
