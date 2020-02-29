#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "out/TTFont.h"
#include "out/SpriteRenderer.h"
#include "out/WireRenderer.h"
#include "../Body.h"
#include "../map/Map.h"
#include "../Brush.h"


class GameWorld : public fk::Scene {
  public:
	fk::World world;
	fk::SpriteBatch* spriteBatchPtr{ nullptr };
	fk::SpriteBatch* textBatchPtr{ nullptr };
	fk::SpriteRenderer spriteRenderer;
	fk::WireRenderer wireRenderer;
	fk::TTFont font;
	fk::Camera cam;
	Brush* brushPtr;
	std::vector<Image*> imagePtrs;
	std::vector<Map> Maps;
	Tile* currentTilePtr{ nullptr };
	std::vector<Tile*> surroundingTilePtrs;


	std::vector<Map> maps;
	GameWorld() = default;
	~GameWorld() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};