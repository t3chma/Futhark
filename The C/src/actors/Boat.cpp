#include "Boat.h"
#include "base/Utility.h"
#include "in/IOManager.h"
#include <set>

Boat::Boat(Boat::Def& bd) : Actor(bd, *(new M_Control(*this))), m_uiPtr(uiPtr) {
	health = bd.health;
	// Make wake textures.
	for(int i = 0; i < _TRAIL_; ++i) {
		sprites.emplace_back(bd.wake.batch, bd.wake.textureFilePath);
		spritePtrs.floors.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = 0;
		sprites.back().setDimensions(bd.size, bd.size);
	}
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
	struct Position {
		Position(int x, int y, int z = 0) : x(x), y(y), z(z) {}
		bool operator <(const Position& rhs) const { return x == rhs.x ? y < rhs.y : x < rhs.x; }
		int x;
		int y;
		int z{ 0 };
	};
	// For checking text neighbors (up, left, right, down).
	std::vector<Position> offsets;
	offsets.reserve(4);
	offsets.emplace_back(0, 1);
	offsets.emplace_back(-2, 0);
	offsets.emplace_back(2, 0);
	offsets.emplace_back(0, -1);
	b2FixtureDef roomDef;
	// For queueing wall generation.
	std::set<Position> wallSet;
	// For each line.
	for (int y = 0; y < boatData.size(); ++y) {
		// For each char.
		for (int x = 0; x < boatData[y].size(); ++x) {
			// Depending on the char.
			switch (boatData[y][x]) {
			  case '\n':
				// Iterate the relative length for the current level.
				// Odd lengths will always be populated with walls so iterate by 2.
				if (length += 2 > shipDimensions.y) { shipDimensions.y = length; }
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
				if (++width > shipDimensions.x) { shipDimensions.x = width; }
				break;
			  case 'I':
			  case 'E':
			  case 'S':
			  case 's':
			  case 'b':
			  case 'x':
			  	// Don't process the ship overview.
			  	if (!hitShip) { break; }
				// Make the physics object.
				addRectangleLimb(1, 1, x, length, 0, &roomDef).category = boatData[y][x];
				sprites.emplace_back(bd.floor.batch, bd.floor.textureFilePath);
				spritePtrs.floors.push_back(&sprites.back());
				sprites.back().getCanvasRef().position.z = level;
				sprites.back().setDimensions(bd.size, bd.size);
				// For each neighbor.
				for (int i = 0; i < offsets.size(); ++i) {
					// If out of bounds treat it like a space.
					int yo = y + offsets[i].y;
					int xo = x + offsets[i].x;
					bool outOfBounds = (yo < 0 || yo >= boatData.size()) && (xo < 0 || xo >= boatData[yo].size());
					char offsetChar = outOfBounds ? ' ' : boatData[y + offsets[i].y][x + offsets[i].x];
					// If your neighbor is not the same as you then queue wall creation.
					if (offsetChar != boatData[y][x]) {
						wallSet.emplace(x + offsets[i].x / 2, length + offsets[i].y, level);
					}
				}
				break;
			  default:
				break;
			}
		}
	}
	// Generate wall sprites.
	for (auto&& position : wallSet) {
		sprites.emplace_back(bd.wall.batch, bd.wall.textureFilePath);
		spritePtrs.walls.push_back(&sprites.back());
		sprites.back().getCanvasRef().position.z = position.z;
		sprites.back().setDimensions(bd.size/8, bd.size);
		// If the wall is at an odd position it should be horizontal
		if (position.y % 2) { sprites.back().setRotation(fk::TAU/4); }
	}
}
Boat::~Boat() {

}
void Boat::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Mouse
	// TODO: Remove this from all boats into a player class or something.
	m_mousePos = camPtr->getWorldCoordinates(p_uiPtr->getMouseInfo(0).windowPosition);
	// States
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
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
	// TODO: Make sprites move relative to ship center. Might work already now.
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
		wakePtr->setPosition(m_oldPos[i].x, m_oldPos[i].y);
		wakePtr->setRotationAxis(m_oldPos[i].x, m_oldPos[i].y);
		wakePtr->setDimensions(glm::vec2(shipDimensions.x) + glm::vec2(0.02 * i));
		wakePtr->setColor(255, 255, 255, (_TRAIL_ * 2 - i * 2) / 4);
		++i;
	}
	states.currentPtr->updateSprite();
}
float32 Boat::ReportFixture(
	b2Fixture* fixturePtr,
	const b2Vec2& point,
	const b2Vec2& normal,
	float32 fraction
) {
	p_raycast.target = static_cast<Object*>(fixturePtr->GetBody()->GetUserData());
	p_raycast.fraction = fraction;
	return fraction;
}


void Boat::M_Control::enter() {
	Boat& player = *static_cast<Boat*>(actorPtr);
	actorPtr->sprites.front().setColor(255, 255, 255, 255);
}
void Boat::M_Control::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	Boat& player = *static_cast<Boat*>(actorPtr);
	b2Vec2 test;
	// Move direction
	if (player.m_uiPtr->getKeyInfo(fk::Key::D).downFrames) {
		player.los.faceDirection = fk::rotatePoint(player.los.faceDirection, -fk::TAU / 500);
	}
	if (player.m_uiPtr->getKeyInfo(fk::Key::A).downFrames) {
		player.los.faceDirection = fk::rotatePoint(player.los.faceDirection, fk::TAU / 500);
	}
	// Throtle
	static int gear = 0;
	if (player.m_uiPtr->getKeyInfo(fk::Key::W).downFrames == 1 && gear < 4) { gear += 1; }
	if (player.m_uiPtr->getKeyInfo(fk::Key::S).downFrames == 1 && gear > -1) { gear -= 1; }
	int shiftFrames = player.m_uiPtr->getKeyInfo(fk::Key::SHIFT_L).downFrames;
	auto direction = glm::normalize(player.los.faceDirection);
	if (gear && (direction.x || direction.y)) {
		float boost{ shiftFrames > 0 ? 0.75f : 0 };
		player.movement.direction = gear * (player.movement.speed + boost) * direction;
	}
	else {
		player.movement.direction = glm::vec2(0);
	}
	fk::Random rangen;
	static int i = 1;
	player.movement.direction.x += rangen.getFloat(0, 0.1) * i;
	player.movement.direction.y += rangen.getFloat(0, 0.1) * i;
	i = -i;
}
void Boat::M_Control::updateBody() {
	State::updateBody();
}
void Boat::M_Control::updateSprite() {
	Boat& player = *static_cast<Boat*>(actorPtr);
}
