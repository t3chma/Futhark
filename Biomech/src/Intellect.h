#pragma once
#include "in/UserInput.h"


class Intellect {
  public:
	virtual void think(fk::UserInput& ui) = 0;
};

