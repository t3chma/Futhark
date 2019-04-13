class Operator {
	// The strength of the primary ability.
	int Power;
	// How far the effects travel.
	enum class Range { HIGH = 3, MEDIUM = 2, LOW = 1, INFINITY = 0} range{ MEDIUM };
	// How the rooms effects are delivered.
	enum class Delivery { SEA = -1, AIR = 0, MORTAR = 1} delivery{ AIR };
	// Delay in activation in frames.
	int delay{ 0 };
	// How much the room heats up per use.
	int heat{ 0 };
	// Minimum amount of time in frames that it takes to setup a room.
	int setup{ 60*10 };
	// Type of overloading.
	enum class Overload {
		GUN, // Adds a gun.
		AIM, // Increases aim.
		CLIP, // Increases clip size.
		GAUGE, // Increases the number of bullets fired.
		POWER, // Increases the power
		SPLASH, // Increases the splash damage
		RANGE // Increases the range
	} overload{ GUN };
	// Clip size for a fully prepared room.
	int clip{ 1 };
	// How long the room's effects last in frames (use for burst fire weapons also)
	int duration{ 1 };
}

class Gun : public Operator {
	// Type of damage done.
	enum class Type { PHYSICAL, HEAT } type{ PHYSICAL };
	// How much the bullets can penetrate through.
	int penetration{ 0 };
	// How fast the bullet travels.
	enum class Speed { HIGH = 3, MEDIUM = 2, LOW = 1, NONE = 0, INSTANT = -1} speed{ MEDIUM };
	// Variance put on the players aim in radians.
	float accuracy{ fk::TAU/400 };
	// Variance put on each bullet in radians.
	float spread{ fk::TAU/400 };
	// How many bullets are made per shot.
	int gauge{ 1 };
	// If the bullets should be connected to anything and how.
	enum class Connection { NONE, GUN, NET, LINE} connection{ NONE };
	// Radius of any splash damage.
	float splash{ 0 };
	// What triggers the bullets.
	enum class Trigger { IMPACT, RANGE} trigger{ IMPACT };
}
