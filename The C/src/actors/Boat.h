#pragma once
#include <set>
#include "Actor.h"
#include "in/UserInput.h"
#include "out/Camera.h"
#include "../Mouse.h"
#include "States.h"
#define _TRAIL_ 100

class Boat : public Actor {
  public:
	struct Def : public Actor::Def {
		Def(fk::World& world, fk::FileCache& textureCache, fk::SpriteBatch& batch, std::string& boatFile) :
			Actor::Def(world, textureCache),
			textureCache(textureCache),
			floor(batch, "deck.png"),
			wall(batch, "wall.png"),
			wake(batch, "wake.png"),
			boatFile(boatFile)
		{}
		Actor::Def::SpriteDef floor;
		Actor::Def::SpriteDef wall;
		Actor::Def::SpriteDef wake;
		std::string& boatFile{""};
		int health{ 100 };
	};
	struct {
		fk::Sprite* art{ nullptr };
		std::vector<fk::Sprite*> floors{ nullptr };
		std::vector<fk::Sprite*> walls{ nullptr };
		std::vector<fk::Sprite*> wakes{ nullptr };
	} spritePtrs;
	Boat(Boat::Def& bd);
	~Boat();
	void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	void updateSprites() override;
	void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	float32 ReportFixture(
		b2Fixture* fixture,
		const b2Vec2& point,
		const b2Vec2& normal,
		float32 fraction
	);
private:
	struct M_Control : State {
		M_Control(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
		virtual void updateBody() override;
		virtual void updateSprite() override;
	};
	fk::UserInput* m_uiPtr{ nullptr };
	glm::vec2 m_mousePos;
	glm::vec2 m_oldPos[_TRAIL_];
	float m_oldAng[_TRAIL_];
	fk::Vec2 m_wakeReset;
	glm::vec2 m_oldVel{ 0 };
};
