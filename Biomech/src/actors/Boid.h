#pragma once
#include "../Body.h"
#include "../Image.h"
#include "../Intellect.h"
#include <set>
class Equipment;


struct Goal {
	glm::vec2 location{ 0 };
	Body* bodyPtr{ nullptr };
	bool resolved{ true };
};

class Boid : public Body, public Image, public Intellect {
  public:
	friend Equipment;
	struct LineOfSight {
		struct Friend {
			Friend(float i, float j, float x, float y) : direction(i, j), position(x, y) {}
			glm::vec2 position;
			glm::vec2 direction;
		};
		LineOfSight(float radius);
		float radius{ 0 };
		float angle{ 0 };
		struct FriendCompare {
			bool operator() (const Friend& lhs, const Friend& rhs) const {
				return
					lhs.direction.x < rhs.direction.x &&
					lhs.direction.y < rhs.direction.y &&
					lhs.position.x < rhs.position.x &&
					lhs.position.y < rhs.position.y;
			}
		};
		std::set<Friend, FriendCompare> friends;
		struct GLMVec2Compare {
			bool operator() (const glm::vec2& lhs, const glm::vec2& rhs) const {
				return lhs.x < rhs.x && lhs.y < rhs.y;
			}
		};
		std::set<glm::vec2, GLMVec2Compare> badPositions;
	} los;
	struct PriorityWeights {
		float studder{ 10 };
		float alignment{ 0.01 };
		float seperation{ 8 };
		float cohesion{ 15 };
		float seek{ 15 };
		float target{ 25 };
		float snap{ 2 };
		float aversion{ 22 };
	} pw;
	struct GoalVectors {
		glm::vec2 alignment{ 0 };
		glm::vec2 seperation{ 0 };
		glm::vec2 cohesion{ 0 };
		glm::vec2 seek{ 0 };
		glm::vec2 target{ 0 };
		glm::vec2 snap{ 0 };
		glm::vec2 aversion{ 0 };
	} gv;
	struct Def {
		Body::Def bd;
		float sightRadius{ 1 };
		Boid::PriorityWeights pw;
	};
	std::vector<Goal> goals;
	glm::vec2 goal{1, 1};
	float damage{ 1 };
	virtual void primeGun(int option);
	virtual void releaseGun();
	virtual void primeGadget(int option);
	virtual void releaseGadget();
	Boid(fk::SpriteBatch& sb, fk::World& world, Def bd);
	~Boid();
	void attack(Body& body);
	virtual glm::vec2 getDirection() final { return p_direction; };
	virtual void think(fk::UserInput& ui) override final;
	virtual void draw() = 0;
	void setGun(Equipment* ePtr);
	Equipment* getGun();
	void setGadget(Equipment* ePtr);
	Equipment* getGatget();
  protected:
	Equipment* p_gunPtr{ nullptr };
	Equipment* p_gadgetPtr{ nullptr };
	float p_speed{ 20 };
	glm::vec2 p_direction{ 0 };
	glm::vec2 p_looking{ 0 };
	virtual void p_think(fk::UserInput& ui);
	virtual void p_preCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2Manifold* oldManifoldPtr
	) override;
	virtual void p_postCollisionAdjusting(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr,
		const b2ContactImpulse* impulsePtr
	) override;
};