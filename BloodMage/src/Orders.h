#pragma once
#include "GLM/vec2.hpp"
#include <vector>
#include <functional>
#include "Object.h"
#include "base/Utility.h"
class Actor;
class Map;


class Order : public Object {
public:
	fk::Random ranGen;
	enum Wanderability { OPEN, FIELD, NONE };
	enum Behavior { IGNORE, ENGAGE, RETREAT };
	Actor* ownerPtr{ nullptr };
	// Hold time in frames
	int holdTime{ 0 };
	// Held time in frames
	int heldTime{ 0 };
	// Speed to node
	float speed{ 1 };
	// Current target
	glm::vec2 target;
	// The random field
	std::vector<glm::vec2> field;
	// Wander setting
	Wanderability wanderability{ NONE };
	// Combat setting
	Behavior behavior{ IGNORE };
	// Conditions to check for this node
	std::vector<std::function<bool(void)>> conditions;
	// How many times this node has been hit
	int hit{ 0 };
	// If the node was reached
	bool in{ false };
	// Next nodes
	std::vector<Order*> nextNodes;
	// Prev nodes
	std::vector<Order*> prevNodes;
	// If the owner is a prev node
	bool prevOwner{ false };
	// Arrow texture
	fk::Texture arrowTexture;
	// Map
	Map* mapPtr;
	Order(
		Map& map,
		std::vector<fk::Texture>& textures,
		glm::vec2& position,
		Actor* actorPtr,
		bool prevOwner = false
	);
	Order* execute(glm::vec2 position);
	Order* getNext();
	bool checkConditions() { return true; };
	void show();
	void hide();
	void addNext(std::vector<fk::Texture>& textures, glm::vec2& position);
};
