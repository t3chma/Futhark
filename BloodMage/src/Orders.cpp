#include "Orders.h"
#include "actors/Actor.h"


Order::Order(
	Map& map,
	fk::Texture& node,
	fk::Texture& arrow,
	glm::vec2& position,
	Actor* actorPtr,
	bool prevOwner
) : Object(map, b2_dynamicBody, position.x, position.y),
	ownerPtr(actorPtr),
	prevOwner(prevOwner),
	map(map),
	target(position),
	arrowTexture(arrow)
{
	// Graphics
	sprites.emplace_back(map.logicSprites, node);
	sprites.front().setDimensions(0.6, 0.6);
	sprites.front().setPosition(position.x, position.y);
	if (prevOwner) {
		sprites.emplace_back(map.logicSprites, arrowTexture);
		sprites.back().makeLine(ownerPtr->getPosition(), position, 0.3);
	}
	b2FixtureDef fixtureDef1;
	fixtureDef1.isSensor = true;
	addCircleLimb(0.3, 0, 0, &fixtureDef1);
	// Misc
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
	if (!sprites.front().getCanvasRef().color.a) {
		for (auto&& sprite : sprites) { sprite.getCanvasRef().color.a = 255; }
		for (auto&& node : nextNodes) { node->show(); }
	}
}

void Order::hide() {
	if (sprites.front().getCanvasRef().color.a) {
		for (auto&& sprite : sprites) { sprite.getCanvasRef().color.a = 0; }
		for (auto&& node : nextNodes) { node->hide(); }
	}
}

void Order::addNext(fk::Texture& node, fk::Texture& arrow, glm::vec2& position) {
	nextNodes.push_back(new Order(map, node, arrow, position, ownerPtr));
	nextNodes.back()->prevNodes.push_back(this);
	// TODO: FIX THIS!!!! Replaces old arrows!
	nextNodes.back()->sprites.emplace_back(map.logicSprites, nextNodes.back()->arrowTexture);
	nextNodes.back()->sprites.back().makeLine(
		getPosition(), nextNodes.back()->getPosition(), 0.3
	);
}