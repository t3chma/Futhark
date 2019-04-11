class Operator {
	// The strength of the primary ability.
	int Power;
}

class Gun : public Operator {
	// Type of damage done.
	enum class Type { PHYSICAL, HEAT } type{ PHYSICAL };
	// How far the bullets travel.
	enum class Range { HIGH = 3, MEDIUM = 2, LOW = 1, INFINITY = 0} range{ MEDIUM };
	// How much the bullets can penetrate through.
	int penetration{ 0 };
	// How fast the bullet travels.
	enum class Speed { HIGH = 3, MEDIUM = 2, LOW = 1, NONE = 0, INSTANT = -1} speed{ MEDIUM };
	// How the bullets are delivered
	enum class Delivery { SEA = -1, AIR = 0, MORTAR = 1} delivery{ AIR };
	// Delay in firing in frames
	int delay;
	// How much the gun heats up per shot
	int heat;
}
