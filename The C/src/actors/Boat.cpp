#include "Boat.h"
#include "base/Utility.h"
#include "in/IOManager.h"
#include <set>

void Boat::m_parseCharacter(
	std::vector<std::string> &boatData,
	int y, int x,
	int &width, int &length, int &level,
	bool &hitShip, bool &hitDeck,
	std::vector<Boat::Position> &offsets,
	std::set<Boat::Position> &wallSet, std::set<Boat::Position> &roomSet
) {
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
		// For each neighbor.
		for (int i = 0; i < offsets.size(); ++i) {
			// If out of bounds treat it like a space.
			int yo = y + offsets[i].y;
			int xo = x + offsets[i].x;
			bool outOfBounds = yo < 0 || yo >= boatData.size() || xo < 0 || xo >= boatData[yo].size() - 1;
			char offsetChar = outOfBounds ? ' ' : boatData[yo][xo];
			// If your neighbor is not the same as you then queue wall creation.
			char wallType = 'f';
			switch (offsetChar) {
			  default:
				if (offsetChar != boatData[y][x]) { wallType = 'i'; }
				break;
			  case '|':
				if (boatData[yo][xo - 1] == ' ' || boatData[yo][xo - 1] == '\n') {
					wallType = 'l';
				} else if (boatData[yo][xo + 1] == ' ' || boatData[yo][xo + 1] == '\n') {
					wallType = 'r';
				} else {
					wallType = 'i';
				}
				break;
			  case ' ':
			  case '-':
			  case '5':
			  case '4':
			  case '3':
			  case '2':
			  case '1':
			  case '0':
				wallType = 'e';
				break;
			}
			wallSet.emplace(Position(x + offsets[i].x / 2, length + offsets[i].y));
		}
		// Queue room creation.
		roomSet.emplace(x, length, level, boatData[y][x]);
		break;
	  default:
		break;
	}
}

Boat::Boat(Boat::Def& bd, State& startState, AgroState* agroStatePtr) : Actor(bd, startState, agroStatePtr) {
	category = "boat";
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
	// For wall and room generation.
	std::set<Position> wallSet;
	std::set<Position> roomSet;
	// For checking text neighbors (up, left, right, down).
	std::vector<Position> offsets;
	offsets.reserve(4);
	offsets.emplace_back(0, 1);
	offsets.emplace_back(-2, 0);
	offsets.emplace_back(2, 0);
	offsets.emplace_back(0, -1);
	b2FixtureDef roomDef;
	// For each line.
	for (int y = 0; y < boatData.size(); ++y) {
		// For each char.
		for (int x = 0; x < boatData[y].size(); ++x) {
			m_parseCharacter(
				boatData,
				y, x,
				width, length, level,
				hitShip, hitDeck,
				offsets,
				wallSet, roomSet
			);
		}
	}
	float size = bd.size / 2;
	glm::vec2 adjustment = glm::vec2(m_shipDimensions.x / 2 * size, m_shipDimensions.y / 2 * size) - glm::vec2(0.25);
	// Generate wake sprites.
	for (int i = 0; i < _TRAIL_; ++i) {
		sprites.emplace_back(bd.wake.batch, bd.textureCache.get(bd.wake.textureFilePath, 1));
		spritePtrs.wakes.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = -0.0001;
		sprites.back().setDimensions(m_shipDimensions.x * size, m_shipDimensions.x * size);
	}
	// Generate rooms.
	for (auto&& position : roomSet) {
		glm::vec2 sPos = glm::vec2((float)position.x * size, (float)position.y * size) - adjustment;
		// Make the physics object.
		auto&& room = addRectangleLimb(size, size, sPos.x, sPos.y, 0, &roomDef);
		room.category = position.r;
		// Make the sprite.
		sprites.emplace_back(bd.floor.batch, bd.textureCache.get(bd.floor.textureFilePath, 1));
		spritePtrs.floors.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = position.z;
		sprites.back().setDimensions(bd.size * 1.02, bd.size * 1.02);
		sprites.back().setPosition(sPos);
		m_rooms.emplace(&room, Room(position.r, &room, &sprites.back()));
	}
	// Generate wall sprites.
	for (auto&& position : wallSet) {
		glm::vec2 sPos = glm::vec2((float)position.x * size, (float)position.y * size) - adjustment;
		bool horizontal = position.y & 1;
		if (horizontal) {
			sprites.emplace_back(bd.wallH.batch, bd.textureCache.get(bd.wallH.textureFilePath, 1));
		} else {
			sprites.emplace_back(bd.wallV.batch, bd.textureCache.get(bd.wallV.textureFilePath, 1));
		}
		spritePtrs.walls.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = position.z;
		if (position.z < 0) { sprites.back().getCanvasRef().color.a = 0; }
		int division = 64;
		float multiply = 1.05;
		switch (position.r) {
		  case 'l':
		  case 'r': multiply /= 2;
		  case 'e': division /= 4; break;
		  case 'i':
		  case 'f':
		  default: sprites.back().getCanvasRef().color.a = 0; break;
		}
		if (horizontal) {
			sprites.back().setDimensions(bd.size * multiply, bd.size / division);
			if (position.r == 'l') { sPos.x -= size / 2; }
			else if (position.r == 'r') { sPos.x += size / 2; }
		} else {
			sprites.back().setDimensions(bd.size / division, bd.size * multiply);
		}
		sprites.back().setPosition(sPos);
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
		wakePtr->setDimensions(glm::vec2(m_shipDimensions.x / 4) + glm::vec2(1.0f/16 * i));
		wakePtr->setColor(255, 255, 255, ((255 - (255.0f / _TRAIL_ * i)) / 4 ));
		++i;
	}
	states.currentPtr->updateSprite();
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

glm::vec2 Boat::getTarget() {
	return m_target;
}

float Boat::Room::getHalfSize() {
	return floorSpritePtr->getCanvasRef().dimensions.x;
}

glm::vec2 Boat::Room::getPosition() {
	return floorSpritePtr->getPosition();
}