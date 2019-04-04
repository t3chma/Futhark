#include "Boat.h"
#include "base/Utility.h"
#include "in/IOManager.h"
#include <map>

Boat::Boat(
	fk::World& world,
	fk::UserInput* uiPtr,
	BoatDef& bd
) : Actor(world, bd.ad, *(new M_Control(*this)), nullptr), m_uiPtr(uiPtr) {
	// Misc
	health = 5000;
	// Graphics
	for (auto&& sprite : sprites) { sprite.setDimensions(bd.ad.size / 2.0f, bd.ad.size / 2.0f); }
	spritePtrs.floor = &sprites.front();
	spritePtrs.floor->getCanvasRef().position.z = 1;
	spritePtrs.floor->setColor(255, 255, 255, 255);
	spritePtrs.floor->setDimensions(bd.ad.size, bd.ad.size);

	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	// AOE attack
	addCircleLimb(1, 0, 0, &fixtureDef).category = "S";
}
Boat::~Boat() {

}
void Boat::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Mouse
	m_mousePos = camPtr->getWorldCoordinates(m_uiPtr->getMouseInfo(0).windowPosition);
	// States
	if (health < 1) { setState(new Dead(*this)); }
	states.currentPtr->think(actorPtrs, camPtr);
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldPos[i] = m_oldPos[i - 1]; }
	m_oldPos[0] = getPosition();
	for (int i = _TRAIL_ - 1; i > 0; --i) { m_oldAng[i] = m_oldAng[i - 1]; }
	auto velocity = b2Ptr->GetLinearVelocity();
	auto vel = glm::vec2(velocity.x, velocity.y);
	if (vel.x || vel.y) {
		m_oldAng[0] = fk::makeAngle(glm::normalize(vel)) - fk::TAU / 4;
		m_oldVel = vel;
	}
	else {
		m_oldAng[0] = m_oldAng[1];
	}
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
	int i = 0;
	for (auto&& sprite : sprites) {
		if (&sprite == &sprites.front()) {
			sprite.setRotationAngle(b2Ptr->GetAngle());
			sprite.setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
			sprite.setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		}
		else {
			sprite.setRotationAngle(m_oldAng[i]);
			sprite.setPosition(m_oldPos[i].x, m_oldPos[i].y);
			sprite.setRotationAxis(m_oldPos[i].x, m_oldPos[i].y);
			sprite.setDimensions(glm::vec2(p_radius) + glm::vec2(0.02 * i));
			auto velocity = b2Ptr->GetLinearVelocity();
			sprite.setColor(255, 255, 255, (100 - i) / 8);
			++i;
		}
	}
	states.currentPtr->updateSprite();
}
void Boat::makeBoatFromFile(std::string& boatFile) {
	// Read in file.
	fk::IOManager iom;
	std::vector<std::string> boatData;
	iom.readTextFileToStringVector("Boats/" + boatFile, boatData);
	// For keeping track of room positioning.
	bool hitShip{ false };
	bool hitDeck{ false };
	int level{ 0 };
	int length{ 0 };
	// For checking text neighbors (up, left, right, down).
	struct Offsets { Offsets(int x, int y) : x(x), y(y) {} int x; int y; };
	std::vector<Offsets> offsets;
	offsets.reserve(4);
	offsets.emplace_back(0, 1);
	offsets.emplace_back(-2, 0);
	offsets.emplace_back(2, 0);
	offsets.emplace_back(0, -1);
	b2FixtureDef roomDef;
	// For queueing wall generation.
	struct wallMapKey {
		roomMapKey(int x, int y) : x(x), y(y) {}
		bool operator <(const Class1& rhs) const { return x == rhs.x ? y < rhs.y : x < rhs.x; }
		int x;
		int y;
	};
	std::map<roomMapKey, bool> wallMap;
	// For each line.
	for (int y = 0; y < boatData.size(); ++y) {
		// For each char.
		for (int x = 0; x < boatData[y].size(); ++x) {
			// Depending on the char.
			switch (boatData[y][x]) {
			  case '\n':
				// Iterate the relative length for the current level.
				// Odd lengths will always be populated with walls.
				length += 2;
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
				addRectangleLimb(1, 1, x, y, 0, &roomDef).category = boatData[y][x];
				// For each neighbor.
				for (int i = 0; i < offsets.size(); ++i) {
					// If out of bounds treat it like a space.
					int yo = y + offsets[i].y;
					int xo = x + offsets[i].x;
					char offsetChar =
						(yo < 0 || yo >= boatData.size()) && (xo < 0 || xo >= boatData[yo].size()) ?
						' ' :
						boatData[y + offsets[i].y][x + offsets[i].x];
					// If your neighbor is not the same as you.
					if (offsetChar != boatData[y][x]) {
						// Queue wall creation
						wallMap[roomMapKey(x + offsets[i].x / 2, y + offsets[i].y)] = true;
					}
				}
				break;
			  default:
				break;
			}
		}
	}
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
