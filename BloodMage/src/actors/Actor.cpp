#include "Actor.h"

int Actor::advances{ 0 };


Actor::Actor(Map& map, ActorDef& ad, State& startState, Actor::AgroState* agroStatePtr) :
	Object(
		map,
		b2_dynamicBody,
		ad.position.x,
		ad.position.y,
		ad.angularDamping,
		ad.linearDamping
	),
	p_radius(ad.size / 2)
{
	// Update
	map.actorPtrs.push_back(this);
	// AI
	states.currentPtr = &startState;
	states.agroStatePtr = agroStatePtr;
	// Misc
	movement.speed = ad.speed;
	category = "actor";
	// Graphics
	sprites.emplace_back(map.dynamicObjectSprites, ad.body);
	sprites.back().setDimensions(ad.size, ad.size);
	sprites.back().setPosition(ad.position.x, ad.position.y);
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
	for (auto&& actorPtr : map.actorPtrs) {
		if (actorPtr == this) {
			actorPtr = map.actorPtrs.back();
			map.actorPtrs.pop_back();
		}
	};
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
float Actor::getFloorCost(Terrain::Floor floor) {
	switch (floor) {
	  case Terrain::Floor::DEV:
	  return p_floorWeights.dev;
	  case Terrain::Floor::WATER:
	  return p_floorWeights.water;
	  case Terrain::Floor::OIL:
	  return p_floorWeights.oil;
	  case Terrain::Floor::TOXIN:
	  return p_floorWeights.toxin;
	  case Terrain::Floor::GRASS:
	  return p_floorWeights.grass;
	  default:
	  return 10;
	}
}
float Actor::getFluidCost(Terrain::Fluid fluid) {
	switch (fluid) {
	  case Terrain::Fluid::WATER:
	  return p_fluidWeights.water;
	  case Terrain::Fluid::OIL:
	  return p_fluidWeights.oil;
	  case Terrain::Fluid::TOXIN:
	  return p_fluidWeights.toxin;
	  default:
	  return 0;
	}
}
float Actor::getVaporCost(Terrain::Vapor floor) {
	switch (floor) {
	  case Terrain::Vapor::FIRE:
	  return p_vaporWeights.fire;
	  case Terrain::Vapor::POISON:
	  return p_vaporWeights.poison;
	  case Terrain::Vapor::STEAM:
	  return p_vaporWeights.steam;
	  default:
	  return 0;
	}
}
void Actor::startAStar(glm::vec2 target) {
	// Reset data.
	p_pathFindingData.frontier.clear();
	p_pathFindingData.done = false;
	p_pathFindingData.gCosts.clear();
	p_pathFindingData.flow.clear();
	glm::vec2 goal = getPosition();
	p_pathFindingData.iGoal.x = round(goal.x);
	p_pathFindingData.iGoal.y = round(goal.y);
	p_pathFindingData.fStart = target;
	WeightedTile startTile(glm::ivec2(round(target.x), round(target.y)), 0.0);
	p_pathFindingData.iStart = startTile.first;
	p_pathFindingData.frontier.push(startTile);
	p_pathFindingData.flow[startTile.first.x][startTile.first.y] = startTile.first;
	p_pathFindingData.gCosts[startTile.first.x][startTile.first.y] = 0;
}
void Actor::advanceAStar() {
	if (advances++ > 64) { return; }
	// Search loop.
	for (int i = 0; !p_pathFindingData.frontier.empty() && i < 16; ++i) {
		// Inspect the tile with the lowest cost.
		WeightedTile current = p_pathFindingData.frontier.top();
		p_pathFindingData.frontier.pop();
		// If the goal was found.
		if (current.first == p_pathFindingData.iGoal) {
			p_pathFindingData.frontier.clear();
			break;
		}
		// Process each neighbor.
		std::array<std::pair<Terrain*, glm::ivec2>, 8> neghbors = map.getNeighborTilePtrs(current.first);
		for (int i = 0; i < 8; ++i) {
			// Will be nullptr if at the map edge.
			if (neghbors[i].first == nullptr) { continue; }
			double gCost = p_pathFindingData.gCosts[current.first.x][current.first.y]
				+ getFloorCost(neghbors[i].first->floor)
				+ getFluidCost(neghbors[i].first->fluid)
				+ getVaporCost(neghbors[i].first->vapor)
				+ neghbors[i].first->health * 100.0;
			// Adjust the corner values.
			if (i < 4 && i % 2 == 0) { gCost *= 1.4; }
			else if (i % 2 != 0) { gCost *= 1.4; }
			// If the tile hasn't been checked or a lower value was found for it.
			if (
				p_pathFindingData.gCosts.find(neghbors[i].second.x) == p_pathFindingData.gCosts.end()
				|| p_pathFindingData.gCosts[neghbors[i].second.x].find(neghbors[i].second.y)
				== p_pathFindingData.gCosts[neghbors[i].second.x].end()
				|| gCost < p_pathFindingData.gCosts[neghbors[i].second.x][neghbors[i].second.y]
			) {
				// Store the new G cost.
				p_pathFindingData.gCosts[neghbors[i].second.x][neghbors[i].second.y] = gCost;
				// Queue the weighted tile.
				double cost = gCost + p_pathFindingData.heuristic(neghbors[i].second, p_pathFindingData.iGoal);
				// Update the path map.
				p_pathFindingData.flow[neghbors[i].second.x][neghbors[i].second.y] = current.first;
				p_pathFindingData.frontier.emplace(neghbors[i].second, cost);
			}
		}
	}
	if (p_pathFindingData.frontier.empty()) {
		p_pathFindingData.done = true;
		p_pathFindingData.staleData = false;
		glm::ivec2 currentTarget(-1);
		if (!p_pathFindingData.path.empty()) { currentTarget = p_pathFindingData.path.front(); }
		p_pathFindingData.path.clear();
		p_pathFindingData.path.emplace_back(
			p_pathFindingData.flow[p_pathFindingData.iGoal.x][p_pathFindingData.iGoal.y]
		);
		while (p_pathFindingData.path.back() != glm::vec2(p_pathFindingData.iStart)) {
			p_pathFindingData.path.emplace_back(
				p_pathFindingData.flow[p_pathFindingData.path.back().x][p_pathFindingData.path.back().y]
			);
			if (glm::vec2(currentTarget) == p_pathFindingData.path.front()) { break; }
		}
		p_pathFindingData.path.emplace_back(p_pathFindingData.fStart);
	}
}
glm::vec2 Actor::getNextPathPointDirection(glm::vec2 targetPos) {
	// Get tile position
	glm::vec2 pos(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	pos.x = round(pos.x);
	pos.y = round(pos.y);
	// If on the next tile in the path remove that tile.
	if (p_pathFindingData.path.size() > 1 && pos == p_pathFindingData.path.front() ) {
		p_pathFindingData.path.pop_front();
	}// Get the vector
	glm::vec2 moveVec = p_pathFindingData.path.front() - pos;
	if (moveVec.x == 0 && moveVec.y == 0 || glm::length(moveVec) > 2) { moveVec = targetPos - pos; }
	return glm::normalize(moveVec);
}
void Actor::markPathStale() {
	p_pathFindingData.staleData = true;
}
const Actor::P_PathFindingData& Actor::getPathFindingData() { return p_pathFindingData; }
void Actor::addOrder(fk::Texture& node, fk::Texture& arrow, glm::vec2& position) {
	orders.all.push_back(new Order(map, node, arrow, position, this, true));
}
void Actor::showNodes() { for (auto&& node : orders.all) { node->show(); } }
void Actor::hideNodes() { for (auto&& node : orders.all) { node->hide(); } }
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
void Actor::updateSprite() {
	sprites.front().setRotationAngle(b2Ptr->GetAngle());
	sprites.front().setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	sprites.front().setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
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


void Actor::State::updateBody() {
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


void Actor::Dead::enter() {
	actorPtr->movement.direction.x = 0;
	actorPtr->movement.direction.y = 0;
	for (auto&& sprite : actorPtr->sprites) { sprite.getCanvasRef().color.a = 0; }
	actorPtr->sprites.front().getCanvasRef().color.a = 100;
}


void Actor::Idle::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: follow orders
	actorPtr->movement.direction.x = 0;
	actorPtr->movement.direction.y = 0;
	if (actorPtr->inLOS(actorPtrs[0], 1)) { actorPtr->setState(actorPtr->states.agroStatePtr->copy()); }
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
}


void Actor::Search::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: search
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	actorPtr->setState(actorPtr->states.agroStatePtr->copy());
}


void Actor::Flee::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: flee
	actorPtr->setState(new Idle(*actorPtr));
}