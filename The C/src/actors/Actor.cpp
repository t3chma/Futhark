#include "Actor.h"
#include "States.h"


Actor::Actor(Actor::Def& ad, State& startState, AgroState* agroStatePtr) :
	Object(
		ad.world,
		b2_dynamicBody,
		ad.position.x,
		ad.position.y,
		ad.angularDamping,
		ad.linearDamping
	),
	p_radius(ad.size / 2),
	p_uiPtr(ad.uiPtr)
{
	// AI
	states.currentPtr = &startState;
	states.agroStatePtr = agroStatePtr;
	// Misc
	movement.speed = ad.speed;
	category = "actor";
	// Graphics. Leave this to children to decide now.
	///for (auto&& sprite : ad.sprites) { sprites.emplace_back(sprite.batch, sprite.texture); }
	///sprites.front().setDimensions(ad.size, ad.size);
	///sprites.front().setPosition(ad.position.x, ad.position.y);
	// Physics
	b2FixtureDef fixtureDef1;
	fixtureDef1.density = ad.density;
	fixtureDef1.friction = ad.friction;
	fixtureDef1.filter.categoryBits = 2;
	addCircleLimb(ad.size / 2, 0, 0, &fixtureDef1);
}
Actor::~Actor() {
	delete states.currentPtr;
	delete states.prevPtr;
	delete states.agroStatePtr;
}
bool Actor::inLOS(Actor* targetPtr, float awareness, std::string ignoreCategory) {
	glm::vec2 targetVec = targetPtr->getPosition() - getPosition();
	float halfAngle = std::abs(
		std::abs(fk::makeAngle(los.faceDirection) -
			fk::makeAngle(glm::normalize(targetVec))) - fk::TAU / 2
	);
	float length(glm::length(targetVec));
	targetInfo.obstructionPtr = nullptr;
	p_raycast.target = targetPtr;
	targetInfo.ptr = nullptr;
	p_raycast.ignore = ignoreCategory;
	if (length < awareness || length < los.distance && halfAngle < los.halfAngle) {
		rayCast(targetPtr->getPosition());
	}
	p_raycast.target = nullptr;
	p_raycast.ignore = "";
	return (targetInfo.obstructionPtr == targetInfo.ptr && targetInfo.ptr == targetPtr);
}
bool Actor::inLOS(glm::vec2 target, float awareness, std::string ignoreCategory) {
	glm::vec2 targetVec = target - getPosition();
	float halfAngle = std::abs(
		std::abs(fk::makeAngle(los.faceDirection) -
			fk::makeAngle(glm::normalize(targetVec))) - fk::TAU / 2
	);
	float length(glm::length(targetVec));
	targetInfo.obstructionPtr = nullptr;
	p_raycast.ignore = ignoreCategory;
	if (length < awareness || length < los.distance && halfAngle < los.halfAngle) {
		b2Ptr->GetWorld()->RayCast(
			this,
			b2Vec2(getPosition().x, getPosition().y),
			b2Vec2(target.x, target.y)
		);
	}
	p_raycast.ignore = "";
	return !targetInfo.obstructionPtr;
}
float Actor::getRadius() { return p_radius; }
float32 Actor::ReportFixture(
	b2Fixture* fixturePtr,
	const b2Vec2& point,
	const b2Vec2& normal,
	float32 fraction
) {
	Object* op = static_cast<Object*>(fixturePtr->GetBody()->GetUserData());
	if (
		(p_raycast.target == fixturePtr->GetBody()->GetUserData() || !p_raycast.target)
		&& op->category == "actor"
	) {
		targetInfo.ptr = static_cast<Actor*>(fixturePtr->GetBody()->GetUserData());
		targetInfo.obstructionPtr = op;
	}
	if (op->category != p_raycast.ignore) {
		targetInfo.obstructionPtr = op;
	}
	return fraction;
}
void Actor::rayCast(glm::vec2 target) {
	rayCast(getPosition(), target);
}
void Actor::rayCast(glm::vec2 origin, glm::vec2 target) {
	b2Ptr->GetWorld()->RayCast(
		this,
		b2Vec2(origin.x, origin.y),
		b2Vec2(target.x, target.y)
	);
}
void Actor::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
}
void Actor::updateBody() { states.currentPtr->updateBody(); }
void Actor::updateSprites() {
	// Leave this to children to decide now.
	///sprites.front().setRotationAngle(b2Ptr->GetAngle());
	///sprites.front().setPosition(getPosition().x, getPosition().y);
	///sprites.front().setRotationAxis(getPosition().x, getPosition().y);
	states.currentPtr->updateSprite();
}
void Actor::setState(State* newStatePtr) {
	delete states.prevPtr;
	states.prevPtr = states.currentPtr;
	states.currentPtr = newStatePtr;
	newStatePtr->enter();
}
void Actor::look(glm::vec2 targetVector) {
	b2Ptr->SetTransform(b2Ptr->GetWorldCenter(), fk::makeAngle(targetVector) + fk::TAU / 4);
}
void Actor::returnToPrevState() {
	std::swap(states.currentPtr, states.prevPtr);
	states.currentPtr->enter();
}
