#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "out/TTFont.h"
#include "out/SpriteRenderer.h"
#include "out/WireRenderer.h"
#include "../Body.h"
#include "../actors/Player.h"
#include "../Arena.h"

class GameWorld : public fk::Scene {
  public:
	Arena* arenaPtr;
	std::vector<std::vector<TextBlock>> map;
	fk::World world;
	fk::SpriteBatch* spriteBatchPtr{ nullptr };
	fk::SpriteBatch* textBatchPtr{ nullptr };
	fk::SpriteRenderer spriteRenderer;
	fk::WireRenderer wireRenderer;
	fk::TTFont font;
	fk::Camera cam;
	std::vector<Intellect*> actorPtrs;
	std::vector<Image*> imagePtrs;
	Player* playerPtr;
	GameWorld() = default;
	~GameWorld() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};