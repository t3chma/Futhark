#include "StateFactory.h"


int Actor::advances{ 0 };


Actor::Actor(Map& map, ActorDef& ad, State& startState, AgroClass agroClass) :
	Object(map.dynamicObjectSprites, map.world, b2_dynamicBody, ad.position.x, ad.position.y),
	map(map),
	p_radius(ad.size / 2)
{
	states.currentPtr = &startState;
	states.agroClass = agroClass;
	ad.size *= 0.6;
	map.actorPtrs.push_back(this);
	spriteIDs.reserve(ad.textures.size());
	for (auto&& texture : ad.textures) {
		spriteIDs.push_back(spriteBatch.makeSprite(texture));
		spriteBatch[spriteIDs.back()].setDimensions(ad.size, ad.size);
		spriteBatch[spriteIDs.back()].setPosition(ad.position);
	}
	b2BodyPtr->SetLinearDamping(10);
	b2BodyPtr->SetAngularDamping(5);
	b2FixtureDef fixtureDef1;
	b2CircleShape shape1;
	fixtureDef1.shape = &shape1;
	shape1.m_radius = ad.size / 2;
	fixtureDef1.userData = nullptr;
	fixtureDef1.density = 10.0f;
	fixtureDef1.friction = 0.3f;
	fixtureDef1.filter.categoryBits = 2;
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "actor";
}
Actor::~Actor() {
	for (auto&& id : spriteIDs) { spriteBatch.destroySprite(id); }
	delete states.currentPtr;
	delete states.prevPtr;
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
		b2BodyPtr->GetWorld()->RayCast(
			this,
			b2Vec2(getPosition().x, getPosition().y),
			b2Vec2(targetPtr->getPosition().x, targetPtr->getPosition().y)
		);
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
		b2BodyPtr->GetWorld()->RayCast(
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
	glm::vec2 pos(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
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
void Actor::addOrder(std::vector<fk::Texture>& textures, glm::vec2& position) {
	orders.all.push_back(new Order(map, textures, position, this, true));
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
void Actor::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
}
void Actor::updateBody() { states.currentPtr->updateBody(); }
void Actor::updateSprite() {
	spriteBatch[spriteIDs[0]].canvas.rotationAngle = b2BodyPtr->GetAngle();
	spriteBatch[spriteIDs[0]].setPosition(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	spriteBatch[spriteIDs[0]].setRotationAxis(b2BodyPtr->GetPosition().x, b2BodyPtr->GetPosition().y);
	states.currentPtr->updateSprite();
}
void Actor::setState(State* newStatePtr) {
	delete states.prevPtr;
	states.prevPtr = states.currentPtr;
	states.currentPtr = newStatePtr;
	newStatePtr->enter();
}
void Actor::look(glm::vec2 targetVector) {
	b2BodyPtr->SetTransform(b2BodyPtr->GetWorldCenter(), fk::makeAngle(targetVector) + fk::TAU / 4);
}
void Actor::returnToPrevState() {
	std::swap(states.currentPtr, states.prevPtr);
	states.currentPtr->enter();
}


void Actor::State::updateBody() {
	actorPtr->b2BodyPtr->ApplyLinearImpulse(
		b2Vec2(actorPtr->movement.vector.x, actorPtr->movement.vector.y),
		actorPtr->b2BodyPtr->GetWorldCenter(),
		true
	);
	actorPtr->look(actorPtr->los.faceDirection);
}


void Actor::Dead::enter() {
	actorPtr->movement.vector.x = 0;
	actorPtr->movement.vector.y = 0;
	for (auto&& spriteID : actorPtr->spriteIDs) { actorPtr->spriteBatch[spriteID].canvas.color.a = 0; }
	actorPtr->spriteBatch[actorPtr->spriteIDs[0]].canvas.color.a = 100;
}


void Actor::Idle::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: follow orders
	actorPtr->movement.vector.x = 0;
	actorPtr->movement.vector.y = 0;
	if (actorPtr->inLOS(actorPtrs[0], 1)) { actorPtr->setState(new Cast(*actorPtr)); }
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
}


void Actor::Cast::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: casting
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	actorPtr->setState(makeState(actorPtr->states.agroClass, *actorPtr));
}


void Actor::Search::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: search
	///if (actorPtr->hit) { actorPtr->setState(new P_Stun(actorPtr)); }
	actorPtr->setState(new Idle(*actorPtr));
}


void Actor::Flee::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// TODO: flee
}