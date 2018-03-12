#pragma once
#include "base/Scene.h"
#include "out/SpriteRenderer.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "in/ActionQueue.h"

class AITester : public fk::Scene {
public:
	fk::SpriteBatch sprites;
	fk::SpriteRenderer spriteRenderer;
	std::vector <fk::SpriteBatch::Sprite> localSprites;
	fk::Camera cam;
	friend class camUp;
	friend class camDown;
	friend class camLeft;
	friend class camRight;

	AITester() = default;
	~AITester() = default;
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;

	glm::vec2 camMovement{ 0 };
	struct : public fk::Action {
		glm::vec2* camMovementPtr;
		void execute() override { camMovementPtr->y = 1; }
		void undo() override { camMovementPtr->y = -1; }
	} camUp;
	struct : public fk::Action {
		glm::vec2* camMovementPtr;
		void execute() override { camMovementPtr->y = -1; }
		void undo() override { camMovementPtr->y = 1; }
	} camDown;
	struct : public fk::Action {
		glm::vec2* camMovementPtr;
		void execute() override { camMovementPtr->x = -1; }
		void undo() override { camMovementPtr->x = 1; }
	} camLeft;
	struct : public fk::Action {
		glm::vec2* camMovementPtr;
		void execute() override { camMovementPtr->x = 1; }
		void undo() override { camMovementPtr->x = -1; }
	} camRight;
};