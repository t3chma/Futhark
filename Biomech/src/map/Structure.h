#pragma once
#include "../Stats.h"

class Structure : public StatEffects {
  public:
	enum Type { WATCH = 0, FORT, ROAD, LIGHTS, CACHE, CAMO };
	void setType(int t) override;
	Structure();
	~Structure();
};