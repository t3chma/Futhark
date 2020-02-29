#pragma once
#include "../Stats.h"

class Vehicle : public StatEffects {
  public:
	enum Type { RECON = 0, };
	void setType(int t) override;
	Vehicle();
	~Vehicle();
};