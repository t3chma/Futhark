#pragma once
#include "../Object.h"
#include "../Map.h"
#include <unordered_map>
#include "boost/heap/fibonacci_heap.hpp"
#include "../Orders.h"


struct ActorDef {
	float speed{ 1 };
	float size{ 1 };
	glm::vec2 position{ 0,0 };
	std::vector<fk::Texture> textures;
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
	Actor() = delete;
	Actor(Map& map, ActorDef& ad);
	~Actor();
	float getFloorCost(Terrain::Floor floor);
	float getFluidCost(Terrain::Fluid fluid);
	float getVaporCost(Terrain::Vapor floor);
	void startAStar(glm::vec2 target);
	void advanceAStar();
	virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) = 0;
	void addOrder(std::vector<fk::Texture>& textures, glm::vec2& position);
	void showNodes();
	void hideNodes();
	void pause(int frames);
	bool isDodging();
	virtual float32 ReportFixture(
		b2Fixture* fixture,
		const b2Vec2& point,
		const b2Vec2& normal,
		float32 fraction
	);
	static int advances;
  protected:
	Order* p_currentOrder;
	std::vector<Order*> p_orders;
	float p_speed{ 1 };
	glm::vec2 p_moveDirection{ 0,1 };
	glm::vec2 p_faceDirection{ 0,1 };
	float p_faceAngle{ 0 };
	Map& p_map;
	int p_pause{ 0 };
	std::list<Actor*> p_attackerPtrList;
	Actor* p_targetPtr{ nullptr };
	Object* p_obstructionPtr{ nullptr };
	glm::vec2 p_lastTargetPos{ 0 };
	int p_dodging{ 0 };
	struct {
		// If the current A* data is up to date.
		// Note: Not being up to date does not mean that it is totally useless though.
		bool upToDate{ false };
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
		glm::ivec2 fStart;
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
		float water = 20;
		float oil = 20;
		float toxin = 20;
	} p_fluidWeights;
	struct {
		float none = 0;
		float fire = 20;
		float poison = 20;
		float steam = 0.1;
	} p_vaporWeights;
	struct {
		float fire;
		float earth;
		float water;
		float air;
	} p_blood;
	struct {
		int right{ 0 };
		int left{ 0 };
	} p_drainCount;
};