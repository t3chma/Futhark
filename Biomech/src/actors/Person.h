#pragma once
#include "../Stats.h"

class Person : public StatEffects {
  public:
	enum Type { WORKER = 0, POLITIC, INFORMANT, MECHANIC, ENGINEER, DOCTOR, OFFICIAL };
	void setType(int t) override;
	Person();
	~Person();
};

