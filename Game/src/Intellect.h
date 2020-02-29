#pragma once
#include "in/UserInput.h"


class Intellect {
  public:
	virtual void update(fk::UserInput& ui) = 0;
};

