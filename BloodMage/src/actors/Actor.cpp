#include "Actor.h"


Actor::Actor(Map& map, ActorDef& ad)
	: Object(map.dynamicObjectSprites, map.world, b2_dynamicBody, ad.position.x, ad.position.y),
	p_map(map) {
	//b2BodyPtr->SetSleepingAllowed(false);
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
	b2BodyPtr->CreateFixture(&fixtureDef1);
	category = "actor";
}
Actor::~Actor() {
	for (auto&& id : spriteIDs) { spriteBatch.destroySprite(id); }
}
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
	p_pathFindingData.upToDate = false;
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
	category = "actor";
}
void Actor::advanceAStar() {
	// Search loop.
	for (int i = 0; !p_pathFindingData.frontier.empty() && i < p_pathFindingData.advances; ++i) {
		// Inspect the tile with the lowest cost.
		WeightedTile current = p_pathFindingData.frontier.top();
		p_pathFindingData.frontier.pop();
		// If the goal was found.
		if (current.first == p_pathFindingData.iGoal) {
			p_pathFindingData.frontier.clear();
			break;
		}
		// Process each neighbor.
		std::array<std::pair<Terrain*, glm::ivec2>, 8> neghbors = p_map.getNeighborTilePtrs(current.first);
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
	//std::cout << p_pathFindingData.frontier.size() << " : " << p_pathFindingData.frontier.empty() << "\n";
	if (p_pathFindingData.frontier.empty()) {
		p_pathFindingData.upToDate = true;
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

void Actor::addOrder(std::vector<fk::Texture>& textures, glm::vec2& position) {
	p_orders.push_back(new Order(p_map, textures, position, this, true));
}

void Actor::showNodes() {
	for (auto&& node : p_orders) { node->show(); }
}

void Actor::hideNodes() {
	for (auto&& node : p_orders) { node->hide(); }
}

void Actor::pause(int frames) {
	p_pause = frames;
}
