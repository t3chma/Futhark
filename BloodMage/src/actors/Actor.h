#pragma once
#include "../Object.h"
#include "../Map.h"
#include <unordered_map>
#include "boost/heap/fibonacci_heap.hpp"
#include "../Orders.h"
class Spell;

struct ActorDef {
	float speed{ 1 };
	float size{ 0.6f };
	glm::vec2 position{ 0,0 };
	fk::Texture body{ 0 };
	float angularDamping{ 5.0f };
	float linearDamping{ 10.0f };
	float friction{ 0.3f };
	float density{ 10.0f };
};

// Comparable class for A* queue.
struct WeightedTile : public std::pair<glm::ivec2, double> {
	WeightedTile(glm::ivec2& coords, double cost) { this->first = coords; this->second = cost; }
};
struct Compare {
	bool operator()(const WeightedTile& n1, const WeightedTile& n2) const { return n1.second > n2.second; }
};

class Actor : public Object, public b2RayCastCallback {
  public:
	struct State {
		Actor* actorPtr{ nullptr };
		State() = delete;
		State(Actor& actor) : actorPtr(&actor) {};
		virtual void enter() {};
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) {};
		virtual void updateBody();
		virtual void updateSprite() {};
	};
	struct AgroState : public State {
		AgroState() = delete;
		AgroState(Actor& actor) : State(actor) {};
		virtual State* copy() = 0;
	};
	struct Dead : public Actor::State {
		Dead(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void updateBody() {};
	};
	struct Idle : Actor::State {
		Idle(Actor& actor) : State(actor) {};
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct Search : Actor::State {
		Search(Actor& actor) : State(actor) {};
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct Flee : Actor::State {
		Flee(Actor& actor) : State(actor) {};
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	struct {
		float fire{ 0 };
		float earth{ 0 };
		float water{ 0 };
		float air{ 0 };
		float* begin() { return &fire; }
		float* end() { return &air; }
	} blood;
	struct {
		State* currentPtr{ nullptr };
		State* prevPtr{ nullptr };
		AgroState* agroStatePtr{ nullptr };
	} states;
	struct {
		Order* current;
		std::vector<Order*> all;
	} orders;
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
	struct {
		std::vector<Spell*> self;
		std::vector<Spell*> melee;
		std::vector<Spell*> gun;
	} enchants;
	struct StatEffect { float resistance{1.0}; float buildup{ 0 }; int time{ 0 }; };
	struct {
		StatEffect oiled;
		struct : public StatEffect { float strength{ 0 }; } buffed;
		StatEffect anchored;
		StatEffect petrified;
		StatEffect invincible;
		StatEffect fallen;
		struct : public StatEffect { float strength{ 0 }; } lubed;
		StatEffect muted;
		StatEffect summoning;
		struct : public StatEffect { glm::vec2 source; float length; } chained;
		StatEffect wet;
		StatEffect frozen;
		StatEffect cleansed;
		StatEffect bonded;
		struct : public StatEffect { float strength; } healing;
		struct : public StatEffect { float strength; } energized;
		struct : public StatEffect { float strength; } slowed;
		struct : public StatEffect { float strength; } bleeding;
		struct : public StatEffect { Actor* markerPtr; } marked;
		struct : public StatEffect { float strength; } sped;
		StatEffect burning;
		struct : public StatEffect { float buildup{ 0 }; int time{ 0 }; glm::vec2 source; } frightened;
		StatEffect highlighted;
		StatEffect distracted;
		StatEffect panicked;
		struct : public StatEffect { Actor* allyPtr; } charmed;
		struct : public StatEffect { Actor* attractorPtr; } fixated;
		struct : public StatEffect { float strength; } strengthened;
		struct : public StatEffect { float strength; } sight;
		struct : public StatEffect { Actor* targetPtr; } tracking;
		struct : public StatEffect { float strength; } weakened;
		struct : public StatEffect { float strength; } hidden;
		StatEffect invisible;
		StatEffect blinded;
		struct : public StatEffect { glm::vec2 vector; } blown;
		StatEffect ethereal;
		StatEffect electrified;
		StatEffect poisoned;
		struct : public StatEffect { glm::vec2 destination; bool ready; } teleporting;
		struct : public StatEffect { float strength; } reaching;
		StatEffect mirroring;
		struct : public StatEffect { glm::vec2 direction; float halfAngle; } shielded;
		StatEffect stunned;
		StatEffect* begin() { return &oiled; }
		StatEffect* end() { return &stunned; }
	} statEffects;
	static int advances;
	Actor() = delete;
	Actor(Map& map, ActorDef& ad, State& startState, Actor::AgroState* agroStatePtr);
	~Actor();
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	virtual void updateBody() override;
	virtual void updateSprite() override;
	void setState(State* newStatePtr);
	void look(glm::vec2 targetVector);
	void returnToPrevState();
	bool inLOS(Actor* targetPtr, float awareness = 0, std::string ignoreCategory = "");
	bool inLOS(glm::vec2 target, float awareness = 0, std::string ignoreCategory = "");
	float getRadius();
	float getFloorCost(Terrain::Floor floor);
	float getFluidCost(Terrain::Fluid fluid);
	float getVaporCost(Terrain::Vapor floor);
	void startAStar(glm::vec2 target);
	void advanceAStar();
	glm::vec2 getNextPathPointDirection(glm::vec2 targetPos);
	void markPathStale();
  protected:
		struct P_PathFindingData;
  public:
	const P_PathFindingData& getPathFindingData();
	void addOrder(fk::Texture& node, fk::Texture& arrow, glm::vec2& position);
	void showNodes();
	void hideNodes();
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
	struct P_PathFindingData {
		// If the current A* data is up to date.
		// Note: Not being up to date does not mean that it is totally useless though.
		bool done{ false };
		// If the current A* data is totally worthless.
		bool staleData{ true };
		// Path map.
		std::unordered_map<int, std::unordered_map<int, glm::ivec2>> flow;
		// Tile G cost Map.
		std::unordered_map<int, std::unordered_map<int, double>> gCosts;
		// Priority queue for tiles which need to be processed.
		boost::heap::fibonacci_heap<WeightedTile, boost::heap::compare<Compare>> frontier;
		// For determining the distance of a tile from the goal.
		static double heuristic(glm::ivec2 a, glm::ivec2 b) {
			double dx = std::abs(a.x - b.x);
			double dy = std::abs(a.y - b.y);
			return (dx * dx + dy * dy) * 2.0;
		};
		// The tile coords this actor is in.
		glm::ivec2 iGoal;
		// The tile coords to the target.
		glm::ivec2 iStart;
		// The coords of the target.
		glm::vec2 fStart;
		// Target data
		std::list<glm::vec2> path{ 0 };
	} p_pathFindingData;
	struct {
		float dev = 1;
		float water = 200;
		float oil = 200;
		float toxin = 200;
		float grass = 1;
	} p_floorWeights;
	struct {
		float none = 0;
		float water = 10;
		float oil = 10;
		float toxin = 10;
	} p_fluidWeights;
	struct {
		float none = 0;
		float fire = 10;
		float poison = 10;
		float steam = 0.1;
	} p_vaporWeights;
	float p_radius;
};