#include "base\Utility.h"
#include "actors\Boat.h"
#include "objects\Bullet.h"
#include "Attribute.h"


class Operator {
  public:
	Operator(Boat& boat, Boat::Room& room) : boat(boat), room(room) {};
	Boat& boat;
	Boat::Room& room;
	// Use function.
	virtual void use() = 0;
	// Update function.
	virtual void update() = 0;
	// The strength of the primary ability.
	int Power{ 1 };
	// How far the effects travel.
	enum class Range { HIGH = 3, MEDIUM = 2, LOW = 1, NA = 0} range{ Range::MEDIUM };
	// How the rooms effects are delivered.
	enum class Delivery { SEA = -1, AIR = 0, MORTAR = 1} delivery{ Delivery::AIR };
	// Target level to hit.
	int targetLevel{ 1 };
	// Delay in activation in frames.
	Attribute delay{ 0 };
	// If this operator is queued for use.
	bool queued{ false };
	// How much the room heats up per use.
	int heatUp{ 0 };
	// How hot the room is.
	int heat{ 0 };
	// Minimum amount of time in frames that it takes to setup a room.
	int setup{ 60*10 };
	// How long this operator has prepared for.
	int prep{ 0 };
	// Type of overloading.
	enum class Overload {
		GUN, // Adds a gun.
		AIM, // Increases aim.
		CLIP, // Increases clip size.
		GAUGE, // Increases the number of bullets fired.
		POWER, // Increases the power
		SPLASH, // Increases the splash damage
		RANGE // Increases the range
	} overload{ Overload::GUN };
	// Clip size for a fully prepared room.
	Attribute clip{ 0 };
	// How long the room's effects last in frames (use for burst fire weapons also)
	Attribute duration{ 1 };
	// Damage done to hull when this room breaks
	int collateral{ 1 };
	// How many times this room needs to be fixed before it is functional
	int broke{ 1 };
	// Health of this room
	Attribute health{ 10 };
	// How many frames this room waits between healing itself when not broken
	Attribute healRestFrames{ 1 };
	// How much this room heals itself over time when not broken
	Attribute healRate{ 1 };
	// How many frames it takes to fix this room
	Attribute fixFrames{ 10 };
	// How many frames this room has to rest after each use
	Attribute useRestFrames{ 10 };
	// How rested a crew member is which effects performance 
	Attribute rest{ 100 };
	// How fast this crew member tires when they do something
	Attribute tireRate{ 1 };
	// How much this crew member eats
	Attribute eatRate{ 1 };
};

class GunMan : public Operator {
  public:
	// Fire function.
	virtual void use() override;
	// Inherited via Operator
	virtual void update() override;
	// Type of damage done.
	enum class Type { PHYSICAL, HEAT } type{ Type::PHYSICAL };
	// How much the bullets can penetrate through.
	Attribute penetration{ 0 };
	// How fast the bullet travels.
	enum class Speed { HIGH = 3, MEDIUM = 2, LOW = 1, NONE = 0, INSTANT = 100} speed{ Speed::MEDIUM };
	// Variance put on the players aim in radians.
	float accuracy{ fk::TAU/400 };
	// Variance put on each bullet in radians.
	float spread{ fk::TAU/400 };
	// How many bullets are made per shot.
	int gauge{ 1 };
	// If the bullets should be connected to anything and how.
	enum class Connection { NONE, GUN, NET, LINE} connection{ Connection::NONE };
	// Radius of any splash damage.
	float splash{ 0 };
	// What triggers the bullets.
	enum class Trigger { IMPACT, RANGE} trigger{ Trigger::IMPACT };
	// Which direction this operator will fire
	enum class Aim { UP, LEFT, RIGHT, DOWN, TARGET } aim;
	std::vector<Bullet> bullets;
};