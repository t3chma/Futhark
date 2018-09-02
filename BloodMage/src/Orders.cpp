#include "Orders.h"
#include "actors/Actor.h"


Order::Order(Map& map, std::vector<fk::Texture>& textures, glm::vec2& position, Actor* actorPtr, bool prevOwner)
	: Object(map.logicSprites, map.world, b2_dynamicBody, position.x, position.y),
	ownerPtr(actorPtr),
	prevOwner(prevOwner),
	mapPtr(&map),
	target(position)
{
	sprites.add("", textures[0]);
	sprites.get("")->setDimensions(0.6, 0.6);
	sprites.get("")->setPosition(position.x, position.y);
	arrowTexture = textures[1];
	if (prevOwner) {
		sprites.add("arrow", arrowTexture);
		sprites.get("arrow")->makeLine(ownerPtr->getPosition(), position, 0.3);
	}
	b2FixtureDef fixtureDef1;
	b2CircleShape shape1;
	fixtureDef1.shape = &shape1;
	fixtureDef1.isSensor = true;
	shape1.m_radius = 0.3;
	fixtureDef1.userData = nullptr;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "order";
	field.push_back(position);
}

Order* Order::execute(glm::vec2 position) {
	if (in && ++heldTime > holdTime) {
		Order* next = getNext();
		if (next) { ++next->hit; }
		in = false;
		return next;
	} else {
		if (wanderability == FIELD && target == position) {
			in = true;
			target = field[ranGen.getInt(0, field.size() - 1)];
		}
		return this;
	}
}

Order* Order::getNext() {
	for (auto&& node : nextNodes) { if (node->checkConditions()) { return node; } }
	return nullptr;
}

void Order::show() {
	if (!sprites.get("")->canvas.color.a) {
		for (auto&& id : sprites.ids) { sprites.batch[id.second].canvas.color.a = 255; }
		for (auto&& node : nextNodes) { node->show(); }
	}
}

void Order::hide() {
	if (sprites.get("")->canvas.color.a) {
		for (auto&& id : sprites.ids) { sprites.batch[id.second].canvas.color.a = 0; }
		for (auto&& node : nextNodes) { node->hide(); }
	}
}

void Order::addNext(std::vector<fk::Texture>& textures, glm::vec2& position) {
	nextNodes.push_back(new Order(*mapPtr, textures, position, ownerPtr));
	nextNodes.back()->prevNodes.push_back(this);
	// TODO: FIX THIS!!!! Replaces old arrows!
	nextNodes.back()->sprites.add("arrow", nextNodes.back()->arrowTexture);
	nextNodes.back()->sprites.get("arrow")->makeLine(
		getPosition(), nextNodes.back()->getPosition(), 0.3
	);
}