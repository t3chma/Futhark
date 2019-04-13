#include "Boat.h"
#include "base/Utility.h"
#include "in/IOManager.h"

Boat::Boat(Boat::Def& bd, State& startState, AgroState* agroStatePtr) : Actor(bd, startState, agroStatePtr) {
	health = bd.health;
	// Make boat from file.
	fk::IOManager iom;
	std::vector<std::string> boatData;
	iom.readTextFileToStringVector("Boats/" + bd.boatFile, boatData);
	// For keeping track of room positioning.
	bool hitShip{ false };
	bool hitDeck{ false };
	int level{ 0 };
	int length{ 0 };
	int width{ 0 };
	// For checking text neighbors (up, left, right, down).
	std::vector<Position> offsets;
	offsets.reserve(4);
	offsets.emplace_back(0, 1);
	offsets.emplace_back(-2, 0);
	offsets.emplace_back(2, 0);
	offsets.emplace_back(0, -1);
	// For wall and room generation.
	std::set<Position> wallSet;
	std::set<Position> roomSet;
	b2FixtureDef roomDef;
	// For each line.
	for (int y = 0; y < boatData.size(); ++y) {
		// For each char.
		for (int x = 0; x < boatData[y].size(); ++x) {
			// Depending on the char.
			switch (boatData[y][x]) {
			  case '\n':
				// Iterate the relative length for the current level.
				// Odd lengths will always be populated with walls so iterate by 2.
				width = 0;
				length += 2;
				if (length > m_shipDimensions.y) { m_shipDimensions.y = length; }
				break;
			  case '5':
			  case '4':
			  case '3':
			  case '2':
			  case '1':
			  case '0':
				// Update level and reset relative length for it.
				hitShip = true;
				level = (int)boatData[y][x] - 48;
				if (level == 0) { hitDeck = true; }
				if (hitDeck) { level *= -1; }
				length = 0;
			  case '-':
				if (++width > m_shipDimensions.x) { m_shipDimensions.x = width; }
				break;
			  case 'I':
			  case 'E':
			  case 'S':
			  case 's':
			  case 'b':
			  case 'x':
			  	// Don't process the ship overview.
			  	if (!hitShip) { break; }
				// Queue room creation.
				roomSet.emplace(x, length, level, boatData[y][x]);
				// For each neighbor.
				for (int i = 0; i < offsets.size(); ++i) {
					// If out of bounds treat it like a space.
					int yo = y + offsets[i].y;
					int xo = x + offsets[i].x;
					bool outOfBounds = yo < 0 || yo >= boatData.size() || xo < 0 || xo >= boatData[yo].size();
					char offsetChar = outOfBounds ? ' ' : boatData[y + offsets[i].y][x + offsets[i].x];
					// If your neighbor is not the same as you then queue wall creation.
					if (offsetChar != boatData[y][x]) {
						wallSet.emplace(Position(x + offsets[i].x / 2, length + offsets[i].y, level, offsetChar == ' ' ? 'e' : 'i'));
					}
				}
				break;
			  default:
				break;
			}
		}
	}
	float size = bd.size / 2;
	glm::vec2 adjustment = glm::vec2(m_shipDimensions.x / 2 * size, m_shipDimensions.y / 2 * size) - glm::vec2(0.25);
	// Generate rooms.
	for (auto&& position : roomSet) {
		glm::vec2 sPos = glm::vec2((float)position.x * size, (float)position.y * size) - adjustment;
		// Make the physics object.
		addRectangleLimb(size, size, sPos.x, sPos.y, 0, &roomDef).category = position.r;
		// Make the sprite.
		sprites.emplace_back(bd.floor.batch, bd.textureCache.get(bd.floor.textureFilePath, 1));
		spritePtrs.floors.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = position.z;
		sprites.back().setDimensions(bd.size, bd.size);
		sprites.back().setPosition(sPos);
	}
	// Generate wall sprites.
	for (auto&& position : wallSet) {
		glm::vec2 sPos = glm::vec2((float)position.x * size, (float)position.y * size) - adjustment;
		bool horizontal = position.y & 1;
		if (horizontal) { sprites.emplace_back(bd.wallH.batch, bd.textureCache.get(bd.wallH.textureFilePath, 1)); }
		else { sprites.emplace_back(bd.wallV.batch, bd.textureCache.get(bd.wallV.textureFilePath, 1)); }
		spritePtrs.walls.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = position.z;
		if (horizontal) { sprites.back().setDimensions(bd.size, bd.size / 16); }
		else { sprites.back().setDimensions(bd.size / 16, bd.size); }
		if (position.r == 'i') { sprites.back().getCanvasRef().color.a = 128; }
		sprites.back().setPosition(sPos);
	}
	// Generate wake sprites.
	for (int i = 0; i < _TRAIL_; ++i) {
		sprites.emplace_back(bd.wake.batch, bd.textureCache.get(bd.wake.textureFilePath, 1));
		spritePtrs.wakes.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = -0.0001;
		sprites.back().setDimensions(m_shipDimensions.x * size, m_shipDimensions.x * size);
	}
}
Boat::~Boat() {

}
void Boat::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// States
	Actor::think(actorPtrs, camPtr);
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldPos[i] = m_oldPos[i - 1]; }
	m_oldPos[0] = getPosition();
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldAng[i] = m_oldAng[i - 1]; }
	auto velocity = b2Ptr->GetLinearVelocity();
	auto vel = glm::vec2(velocity.x, velocity.y);
	if (vel.x || vel.y) { m_oldAng[0] = fk::makeAngle(glm::normalize(vel)) - fk::TAU / 4; }
	else { m_oldAng[0] = m_oldAng[1]; }
}
void Boat::p_beginCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
}
void Boat::p_endCollision(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr
) {
}
void Boat::updateSprites() {
	// TODO: Support artwork movement.
	// Move floors.
	for (auto&& floorPtr : spritePtrs.floors) {
		floorPtr->move(m_oldPos[0] - m_oldPos[1]);
		floorPtr->setRotationAngle(b2Ptr->GetAngle());
		floorPtr->setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	}
	// Move walls.
	for (auto&& wallPtr : spritePtrs.walls) {
		wallPtr->move(m_oldPos[0] - m_oldPos[1]);
		wallPtr->setRotationAngle(b2Ptr->GetAngle());
		wallPtr->setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
	}
	// Move wakes.
	int i = 0;
	for (auto&& wakePtr : spritePtrs.wakes) {
		wakePtr->setRotationAngle(m_oldAng[i]);
		wakePtr->setPosition(m_oldPos[i]);
		wakePtr->setRotationAxis(m_oldPos[i]);
		wakePtr->setDimensions(glm::vec2(m_shipDimensions.x / 2) + glm::vec2(0.02 * i));
		wakePtr->setColor(255, 255, 255, (_TRAIL_ * 2 - i * 2) / 4);
		++i;
	}
	states.currentPtr->updateSprite();
}
