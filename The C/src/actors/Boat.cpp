#include "Boat.h"
#include "base/Utility.h"
#include "in/IOManager.h"
#include <set>

Boat::Boat(Boat::Def& bd, fk::UserInput* uiPtr) :
	Actor(bd, *(new M_Control(*this))), m_uiPtr(uiPtr)
{
	// Misc
	health = 5000;
	// Graphics
	// Generate from .boat file now.
	///for (auto&& sprite : sprites) { sprite.setDimensions(bd.ad.size / 2.0f, bd.ad.size / 2.0f); }
	// Physics
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	// AOE attack
	//addCircleLimb(1, 0, 0, &fixtureDef).category = "S";
}
Boat::~Boat() {

}
void Boat::think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr) {
	// Mouse
	// TODO: remove this from all boats into a player class or something.
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
		// TODO: Make sprites move relative to ship center. This might already work...
		//if (&sprite == &sprites.front()) {
		//	sprite.setRotationAngle(b2Ptr->GetAngle());
		//	sprite.setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		//	sprite.setRotationAxis(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
		//} else {
		//	sprite.setRotationAngle(m_oldAng[i]);
		//	sprite.setPosition(m_oldPos[i].x, m_oldPos[i].y);
		//	sprite.setRotationAxis(m_oldPos[i].x, m_oldPos[i].y);
		//	sprite.setDimensions(glm::vec2(p_radius) + glm::vec2(0.02 * i));
		//	auto velocity = b2Ptr->GetLinearVelocity();
		//	sprite.setColor(255, 255, 255, (100 - i) / 8);
		//	++i;
		//}
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
	struct Position {
		Position(int x, int y) : x(x), y(y) {}
		bool operator <(const Position& rhs) const { return x == rhs.x ? y < rhs.y : x < rhs.x; }
		int x;
		int y;
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
				addRectangleLimb(1, 1, x, length, 0, &roomDef).category = boatData[y][x];
				// TODO: factor in level data and add sprite data.
				sprites.emplace_back(bd.floor.batch, bd.floor.texture);
				spritePtrs.floors.push_back(&sprites.back());
				sprites.back().getCanvasRef().position.z = level;
				sprites.back().setDimensions(bd.size, bd.size);
				///spritePtrs.floor->setColor(255, 255, 255, 255);
				// For each neighbor.
				for (int i = 0; i < offsets.size(); ++i) {
					// If out of bounds treat it like a space.
					int yo = y + offsets[i].y;
					int xo = x + offsets[i].x;
					bool outOfBounds = (yo < 0 || yo >= boatData.size()) && (xo < 0 || xo >= boatData[yo].size());
					char offsetChar = outOfBounds ? ' ' : boatData[y + offsets[i].y][x + offsets[i].x];
					// If your neighbor is not the same as you then queue wall creation.
					if (offsetChar != boatData[y][x]) { wallSet.emplace(x + offsets[i].x / 2, length + offsets[i].y); }
				}
				break;
			  default:
				break;
			}
		}
	}
	// Generate walls.
	for (auto&& position : wallSet) {
		sprites.emplace_back(bd.wall.batch, bd.wall.texture);
		spritePtrs.walls.push_back(&sprites.back());
		// TODO: factor in level to position.
		sprites.back().getCanvasRef().position.z = 1;
		sprites.back().setDimensions(bd.size/8, bd.size);
		// If the wall is at an odd position it should be horizontal
		if (position % 2) { sprites.back().setRotation(fk::TAU/4); }
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
