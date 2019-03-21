#include "States.h"
#include "Actor.h"

void State::updateBody() {
	actorPtr->b2Ptr->ApplyLinearImpulse(
		b2Vec2(
			actorPtr->movement.direction.x * actorPtr->movement.speed,
			actorPtr->movement.direction.y * actorPtr->movement.speed
		),
		actorPtr->b2Ptr->GetWorldCenter(),
		true
	);
	actorPtr->look(actorPtr->los.faceDirection);
}


void Dead::enter() {
	actorPtr->movement.direction.x = 0;
	actorPtr->movement.direction.y = 0;
	for (auto&& sprite : actorPtr->sprites) { sprite.getCanvasRef().color.a = 0; }
	actorPtr->sprites.front().getCanvasRef().color.a = 100;
}


void Idle::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: follow orders
	actorPtr->movement.direction.x = 0;
	actorPtr->movement.direction.y = 0;
	if (actorPtr->inLOS(actorPtrs[0], 1)) { actorPtr->setState(actorPtr->states.agroStatePtr->copy()); }
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
}


void Search::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: search
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	actorPtr->setState(actorPtr->states.agroStatePtr->copy());
}


void Flee::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: flee
	actorPtr->setState(new Idle(*actorPtr));
}