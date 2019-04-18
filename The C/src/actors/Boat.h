#pragma once
#include <map>
#include "Actor.h"
#include "../Operator.h"
#define _TRAIL_ 100

class Boat : public Actor {
  public:
	struct Def : public Actor::Def {
		Def() = delete;
		Def(fk::World& world, fk::TextureCache& textureCache, fk::SpriteBatch& batch, std::string& boatFile) :
			Actor::Def(world, textureCache),
			floor(batch, "deck.png"),
			wallV(batch, "wallV.png"),
			wallH(batch, "wallH.png"),
			wake(batch, "wake.png"),
			boatFile(boatFile)
		{}
		Actor::Def::SpriteDef floor;
		Actor::Def::SpriteDef wallV;
		Actor::Def::SpriteDef wallH;
		Actor::Def::SpriteDef wake;
		std::string boatFile{""};
		int health{ 100 };
	};
	struct Position {
		Position(int x, int y, int z = 0, char r = ' ') : x(x), y(y), z(z), r(r) {}
		bool operator < (const Position& rhs) const { return x == rhs.x ? y < rhs.y : x < rhs.x; }
		int x;
		int y;
		int z{ 0 };
		char r{ ' ' };
	};
	struct Room {
		Room() = delete;
		Room(char type, fk::Body::Limb* limbPtr, fk::Sprite* floorSpritePtr) : type(type), limbPtr(limbPtr), floorSpritePtr(floorSpritePtr) {}
		char type{ '*' };
		fk::Body::Limb* limbPtr{ nullptr };
		fk::Sprite* floorSpritePtr{ nullptr };
		Operator* operatorPtr{ nullptr };
	};
	struct {
		fk::Sprite* art{ nullptr };
		std::vector<fk::Sprite*> floors;
		std::vector<fk::Sprite*> walls;
		std::vector<fk::Sprite*> wakes;
	} spritePtrs;
	Boat(Boat::Def& bd, State& startState, AgroState* agroStatePtr = nullptr);
	virtual ~Boat();
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	virtual void updateSprites() override;
	virtual void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	virtual void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
private:
	// TODO: Use a circular buffer instead of this garbage. Boost has a nice one.
	glm::vec2 m_oldPos[_TRAIL_];
	float m_oldAng[_TRAIL_];
	// The ship dimensions.
	glm::vec2 m_shipDimensions{ 0 };
	std::map<fk::Body::Limb*, Room> m_rooms;
};
