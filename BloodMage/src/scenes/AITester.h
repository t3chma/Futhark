#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "../actors/Spear.h"
#include "../actors/Grunt.h"
#include "../actors/Player.h"
#include "../props/Prop.h"
#include "../Map.h"
#include "../Mouse.h"
#include <functional>

class AITester : public fk::Scene {
  public:
	enum Placer {
		NONE,
		FLOOR_NONE, FLOOR_DEV, FLOOR_GRASS, FLOOR_DIRT, FLOOR_WATER, FLOOR_OIL, FLOOR_TOXIN,
		FLUID_NONE, FLUID_WATER, FLUID_OIL, FLUID_TOXIN,
		VAPOR_NONE, VAPOR_FIRE, VAPOR_POISON, VAPOR_STEAM,
		OBJECT_NONE, OBJECT_WALL, OBJECT_DOOR, OBJECT_TREE,
		PROP_NONE, PROP_BOX,
		ACTOR_NONE, ACTOR_GRUNT, ACTOR_SPEAR,
		BEHAVIOR
	};
	int rightBind{ 0 };
	int rightInfo{ 0 };
	int leftBind{ 0 };
	int leftInfo{ 0 };
	Mouse* mousePtr{ nullptr };
	Map map;
	fk::Camera cam;
	AITester() = default;
	~AITester() = default;
	void getCommandLine(fk::Tools& tools);
	void edit(fk::Tools& tools, int downFrames, int bind, int info);
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;
};