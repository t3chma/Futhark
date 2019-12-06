#include "Equipment.h"
#include "../actors/Boid.h"


Equipment::Equipment(fk::SpriteBatch& sb, Def ed) : Image(sb) {
	sprites.emplace_back(p_spriteBatch, ed.body);
	sprites.back().setDimensions(0.4, 0.4);
	sprites.emplace_back(p_spriteBatch, ed.cursorAccesory);
	sprites.back().setDimensions(0.5, 0.5);
	sprites.back().getCanvasRef().color.a = 0;
	sprites.back().getCanvasRef().position.z = 100;
	properties.projectileSpawner = ed.projectileSpawner;
	properties.projectileSpawner.antiTunneling = true;
	properties.projectileSpawner.makerPtr = this;
}

void Equipment::prime(int option, Boid* userPtr, Target target) {
	this->target = target;
	this->option = option;
	this->userPtr = userPtr;
	properties.primed = true;
	return p_prime();
}

Projectile* Equipment::update(fk::UserInput& ui, Target target) {
	Projectile* ret = nullptr;
	// Remove dead projectiles
	std::list<Projectile*> removePtrs;
	for (auto&& projectilePtr : projectilePtrs) {
		if (projectilePtr->properties.lifetime > 0) { --projectilePtr->properties.lifetime; }
		else { removePtrs.push_back(projectilePtr); }
		projectilePtr->think(ui);
	}
	for (auto&& projectilePtr : removePtrs) {
		delete projectilePtr;
		projectilePtrs.remove(projectilePtr);
	}
	if (gun) {
		// Check for reload
		if (properties.loading && --properties.loading.current == 0) {
			properties.clip.current += properties.loading.amount;
			if (properties.clip.current > properties.clip.full) { properties.clip.current = properties.clip.full; }
		}
		// Stabilize
		if (properties.kick.current > 0) { properties.kick.current -= properties.kick.power / 32; }
		if (properties.kick.current < 0) { properties.kick.current += properties.kick.power / 32; }
		// Trigger
		if (
			properties.triggered && properties.charged && properties.clip && userPtr
			&& !properties.bursting && !properties.bursting.cooldown.current
			) {
			properties.bursting.size.current = properties.bursting.size.full;
		}
		// Fire
		if (properties.bursting) {
			if (properties.bursting.delay.current) {
				--properties.bursting.delay.current;
			}
			else if (properties.bursting.cooldown.current) {
				--properties.bursting.cooldown.current;
			}
			else {
				if (--properties.bursting.size.current == 0) {
					properties.bursting.cooldown.current = properties.bursting.cooldown.full;
				}
				else {
					properties.bursting.delay.current = properties.bursting.delay.full;
				}
				userPtr->heat(properties.heatGeneration);
				if (properties.clip.current > 0) { properties.clip.current; }
				properties.kick.current += rangen.getFloat(-properties.kick.power, properties.kick.power);
				properties.projectileSpawner.position = userPtr->b2Ptr->GetPosition() + b2Vec2(.3, .3);
				const b2Vec2 launch = b2Vec2(
					properties.delivery * userPtr->getDirection().x,
					properties.delivery * userPtr->getDirection().y
				);
				for (int i = properties.gauge; i; --i) {
					projectilePtrs.push_back(new Projectile(p_spriteBatch, userPtr->world, properties.projectileSpawner));
					projectilePtrs.back()->b2Ptr->ApplyForceToCenter(
						fk::rotatePoint(
							launch, properties.kick.current + rangen.getFloat(-properties.spread, properties.spread)
						), true
					);
				}
				ret = projectilePtrs.back();
			}
		}
	}
	p_update(target);
	return ret;
}

void Equipment::release() {
	p_release();
	this->target = Target();
	properties.primed = false;
}

void Equipment::reload() {
	properties.loading.current = properties.loading.full;
}

void Equipment::draw() {
	if (userPtr) { sprites.front().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y); }
	for (auto&& projectilePtr : projectilePtrs) { projectilePtr->draw(); }
	p_draw();
}

void ScanMarker::p_prime() {
	if (userPtr) { userPtr->statEffects.markPtr = nullptr; }
}

Projectile* ScanMarker::p_update(Target target) {
	if (properties.primed && target.bodyPtr && this->target.bodyPtr == target.bodyPtr) { ++properties.charged.current; }
	else { this->target = target; properties.charged.current = 0; }
	return nullptr;
}

void ScanMarker::p_release() {
	if (properties.charged) { userPtr->statEffects.markPtr = target.bodyPtr; }
	properties.charged.current = 0;
}

void ScanMarker::p_draw() {
	if (userPtr && userPtr->statEffects.markPtr) {
		sprites.back().setPosition(
			glm::vec2(
				userPtr->statEffects.markPtr->b2Ptr->GetPosition().x,
				userPtr->statEffects.markPtr->b2Ptr->GetPosition().y
			)
		);
		float a =
			(255.0 / properties.charged.full)
			* (properties.charged.current > properties.charged.full ? properties.charged.full : properties.charged.current);
		sprites.back().setColor(255, 0, 0, 255);
	} else if (properties.primed && target.bodyPtr) {
		sprites.back().setPosition(target.bodyPtr->b2Ptr->GetPosition().x, target.bodyPtr->b2Ptr->GetPosition().y);
		float a =
			(255.0 / properties.charged.full)
			* (properties.charged.current > properties.charged.full ? properties.charged.full : properties.charged.current);
		sprites.back().setColor(255, 0, 0, (char)a);
	} else {
		sprites.back().setColor(255, 0, 0, 0);
	}
	Equipment::draw();
}

void Retracer::p_prime() {
	target.location = glm::vec2(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
}

void Retracer::p_release() {
	if (userPtr) { userPtr->b2Ptr->SetTransform(b2Vec2(target.location.x, target.location.y), 0); }
}

void Retracer::p_draw() {
	if (properties.primed) {
		sprites.back().getCanvasRef().color.a = 255;
		sprites.back().setPosition(target.location);
	} else {
		sprites.back().getCanvasRef().color.a = 0;
	}
}

void Booster::p_prime() {
}

void Booster::p_release() {
	if (userPtr) { userPtr->statEffects.speedBoost = 50.0; }
}

void Booster::p_draw() {
	if (properties.primed && userPtr) {
		sprites.back().getCanvasRef().color.a = 255;
		auto b = userPtr->b2Ptr->GetPosition();
		sprites.back().setPosition(b.x, b.y);
	} else {
		sprites.back().getCanvasRef().color.a = 0;
	}
}

void SFCharge::p_prime() {
	properties.charged.current = 0;
	properties.triggered = false;
	target.location.x = userPtr->b2Ptr->GetPosition().x;
	target.location.y = userPtr->b2Ptr->GetPosition().y;
}

Projectile* SFCharge::p_update(Target target) {
	++properties.charged.current;
	if (properties.charged && projectilePtrs.size() && properties.triggered) {
		auto charge = static_cast<SField*>(projectilePtrs.back());
		charge->triggered = true;
		charge->sprites.front().setColor(255, 255, 255, 200);
		charge->properties.lifetime = 5 * 60;
	}
	if (userPtr && properties.primed && projectilePtrs.size() == 0) {
		auto pd = properties.projectileSpawner;
		pd.lifetime = -1;
		pd.position = target.location;
		projectilePtrs.push_back(new SField(p_spriteBatch, userPtr->world, pd));
		properties.triggered = false;
		properties.charged.current = 0;
		return projectilePtrs.back();
	}
	return nullptr;
}

void SFCharge::p_release() {
	properties.triggered = true;
}

void HealField::p_prime() {
	properties.triggered = true;
}

Projectile* HealField::p_update(Target target) {
	++properties.charged.current;
	if (userPtr && properties.triggered) {
		auto pd = properties.projectileSpawner;
		pd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new HField(p_spriteBatch, userPtr->world, pd));
		properties.triggered = false;
		properties.charged.current = 0;
		return projectilePtrs.back();
	}
	return nullptr;
}

void HealField::p_release() {
	properties.triggered = false;
}

void DetectorDust::p_prime() {
	properties.triggered = true;
}

Projectile* DetectorDust::p_update(Target target) {
	sprites.back().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
	++properties.charged.current;
	if (userPtr && properties.triggered) {
		auto pd = properties.projectileSpawner;
		pd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new DDust(p_spriteBatch, userPtr->world, pd));
		properties.triggered = false;
		properties.charged.current = 0;
		return projectilePtrs.back();
	}
	return nullptr;
}

void DetectorDust::p_release() {
	properties.triggered = false;
}

void CPC::p_prime() {
	properties.triggered = true;
}

Projectile* CPC::p_update(Target target) {
	sprites.back().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
	++properties.charged.current;
	if (userPtr && properties.triggered) {
		auto pd = properties.projectileSpawner;
		pd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new CPField(p_spriteBatch, userPtr->world, pd));
		properties.triggered = false;
		properties.charged.current = 0;
		return projectilePtrs.back();
	}
	return nullptr;
}

void CPC::p_release() {
	properties.triggered = false;
}

void Cloak::p_prime() {
	for (auto&& sprite : userPtr->sprites) { sprite.setColor(0,0,0,0); }
	for (auto&& sprite : sprites) { sprite.setColor(0, 0, 0, 0); }
}

void Cloak::p_release() {
	for (auto&& sprite : userPtr->sprites) { sprite.setColor(255, 255, 255, 255); }
	for (auto&& sprite : sprites) { sprite.setColor(255, 255, 255, 255); }
}

void Anchor::p_prime() {
	properties.triggered = true;
}

Projectile* Anchor::p_update(Target target) {
	sprites.back().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
	++properties.charged.current;
	if (userPtr && properties.triggered) {
		auto pd = properties.projectileSpawner;
		pd.position = userPtr->b2Ptr->GetPosition() + b2Vec2(.3,.3);
		projectilePtrs.push_back(new Chain(p_spriteBatch, userPtr->world, pd));
		properties.triggered = false;
		properties.charged.current = 0;
		return projectilePtrs.back();
	}
	return nullptr;
}

void Anchor::p_release() {
	properties.triggered = false;
}

void ShieldWall::p_prime() {
	properties.triggered = true;
}

void ShieldWall::p_release() {
	properties.triggered = true;
}

Projectile* ShieldWall::p_update(Target target) {
	properties.triggered = false;
	return nullptr;
}

void ShieldWall::p_draw() {
	sprites.back().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
}