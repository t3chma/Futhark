#pragma once
#include "map/Map.h"
#include "out/Camera.h"

class Brush : public Image {
  public:
	bool edit{ false };
	glm::vec2 camPos;
	fk::Camera& cam;
	Tile tile;
	Brush(fk::Camera& cam, fk::SpriteBatch& sb, fk::TextureCache& tc);
	~Brush();
	void draw() override;
};

