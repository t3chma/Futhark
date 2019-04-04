#pragma once
#include "../Object.h"
struct State;
struct AgroState;

struct ActorDef {
	float speed{ 0.5 };
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	struct Sprite {
		Sprite() = delete;
		Sprite(fk::SpriteBatch& batch, fk::Texture& texture) :
			texture(texture), batch(batch) {};
		fk::Texture& texture;
		fk::SpriteBatch& batch;
	};
	std::list<Sprite> sprites;
	float angularDamping{ 5.0f };
	float linearDamping{ 2.0f };
	float friction{ 0.3f };
	float density{ 10.0f };
};

class Actor : public Object, public b2RayCastCallback {
  public:
	Actor() = delete;
	Actor(fk::World& world, ActorDef& ad, State& startState, AgroState* agroStatePtr);
	~Actor();
	struct {
		State* currentPtr{ nullptr };
		State* prevPtr{ nullptr };
		AgroState* agroStatePtr{ nullptr };
	} states;
	struct {
		float speed{ 1 };
		glm::vec2 direction{ 0,0 };
		bool dodging{ false };
	} movement;
	struct {
		glm::vec2 faceDirection{ 0,1 };
		double halfAngle{ fk::TAU / 4 };
		float distance{ 7 };
	} los;
	struct {
		glm::vec2 lastKnownPos{ 0 };
		Actor* ptr{ nullptr };
		Object* obstructionPtr{ nullptr };
	} targetInfo;
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	virtual void updateBody() override;
	virtual void updateSprites() override;
	void setState(State* newStatePtr);
	void look(glm::vec2 targetVector);
	void returnToPrevState();
	bool inLOS(Actor* targetPtr, float awareness = 0, std::string ignoreCategory = "");
	bool inLOS(glm::vec2 target, float awareness = 0, std::string ignoreCategory = "");
	float getRadius();
  public:
	virtual float32 ReportFixture(
		b2Fixture* fixture,
		const b2Vec2& point,
		const b2Vec2& normal,
		float32 fraction
	) override;
	void rayCast(glm::vec2 target);
	void rayCast(glm::vec2 origin, glm::vec2 target);
  protected:
	struct {
		std::string ignore{ "" };
		Object* target{ nullptr };
		float fraction{ 0 };
	} p_raycast;
	float p_radius;
};