#pragma once

class StatEffects {
  public:
	int impedimentP{ 0 };
	int impedimentV{ 0 };
	bool supplies{ 0 };
	int visibilityP{ 0 };
	int visibilityV{ 0 };
	int los{ 0 };
	int sight{ 0 };
	int night{ 0 };
	int attackP{ 0 };
	int attackV{ 0 };
	int healthP{ 0 };
	int healthV{ 0 };
	int armorP{ 0 };
	int armorV{ 0 };
	int mood{ 0 };
	int coerce{ 0 };
	bool foundable{ false };
	void reset() {
		impedimentP = 0;
		impedimentV = 0;
		supplies = 0;
		visibilityP = 0;
		visibilityV = 0;
		sight = 0;
		night = 0;
		attackP = 0;
		attackV = 0;
		healthP = 0;
		healthV = 0;
		armorP = 0;
		armorV = 0;
		mood = 0;
		coerce = 0;
		foundable = false;
	}
	StatEffects& operator = (const StatEffects& rhs) {
		this->impedimentP = rhs.impedimentP;
		this->impedimentV = rhs.impedimentV;
		this->supplies = rhs.supplies;
		this->visibilityP = rhs.visibilityP;
		this->visibilityV = rhs.visibilityV;
		this->sight = rhs.sight;
		this->night = rhs.night;
		this->attackP = rhs.attackP;
		this->attackV = rhs.attackV;
		this->healthP = rhs.healthP;
		this->healthV = rhs.healthV;
		this->armorP = rhs.armorP;
		this->armorV = rhs.armorV;
		this->mood = rhs.mood;
		this->coerce = rhs.coerce;
		this->foundable = rhs.foundable;
		return *this;
	}
};