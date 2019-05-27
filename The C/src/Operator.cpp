#include "Operator.h"


void GunMan::use() {
	if (prep / setup) { queued = true; }
}

void GunMan::update() {
	++prep;
	for (auto&& bullet : bullets) {
		bool triggered{ false };
		bool kill{ false };
		if ((int)range * 60 <= ++bullet.lifetime) {
			kill = true;
			if (trigger == Trigger::RANGE) { triggered = true; }
		}
		if (penetration < bullet.hitFixturePtrs.size()) {
			kill = true;
			if (trigger == Trigger::IMPACT) { triggered = true; }
		}
		if (kill) {
			bullets.back().moveTo(bullet);
			bullets.pop_back();
		}

	}
	if (queued && !clip.isMin()) {
		// If preparing.
		if (!delay.isMin()) 
			--delay;
			return;
		}
		--clip;
		// Else fire
		heat += heatUp;
		glm::vec2 trueAim = glm::normalize(boat.los.faceDirection);
		glm::vec2 position = room.getPosition();
		switch (aim) {
		  default: trueAim = glm::normalize(boat.getTarget() - position); break;
		  case Aim::UP: break;
		  case Aim::LEFT: trueAim = fk::rotatePoint(trueAim, fk::TAU / 4); break;
		  case Aim::RIGHT: trueAim = fk::rotatePoint(trueAim, -fk::TAU / 4); break;
		  case Aim::DOWN: trueAim = fk::rotatePoint(trueAim, fk::TAU / 2); break;
		}
		fk::Random rangen;
		trueAim = fk::rotatePoint(trueAim, rangen.getFloat(0, accuracy));
		for (int i = gauge; i; --i) {
			trueAim = fk::rotatePoint(trueAim, rangen.getFloat(0, spread));
			bullets.emplace_back(
				boat.world,
				position + trueAim * room.getHalfSize(),
				trueAim * (float)speed
			);
		}
		if (clip.isMin()) {
			delay.maximize();
			prep = 0;
			queued = false;
		}
	}
}