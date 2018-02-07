#pragma once
#include "base/Scene.h"

class AITester : public fk::Scene {
public:
	AITester();
	~AITester();

	// Inherited via fk::Scene
	virtual void open() override;
	virtual void close() override;
	virtual void update(fk::GameState& gameState) override;
};

