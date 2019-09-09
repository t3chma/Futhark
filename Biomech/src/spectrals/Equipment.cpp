#include "Equipment.h"
#include "../actors/Boid.h"


Equipment::Equipment(fk::SpriteBatch& sb, Equipment::Def& ed) : Image(sb), projectileSpawner(ed.projectileSpawner){
	sprites.emplace_back(p_spriteBatch, ed.body);
	sprites.back().setDimensions(0.4, 0.4);
	sprites.emplace_back(p_spriteBatch, ed.cursorAccesory);
	sprites.back().setDimensions(0.5, 0.5);
	sprites.back().getCanvasRef().color.a = 0;
	sprites.back().getCanvasRef().position.z = 100;
	projectileSpawner.makerPtr = this;
}

Projectile* Equipment::prime(int option, Boid* userPtr, Target target) {
	this->target = target;
	this->option = option;
	this->userPtr = userPtr;
	primed = true;
	return p_prime();
}

Projectile* Equipment::update(fk::UserInput& ui, Target target) {
	std::list<Projectile*> removePtrs;
	for (auto&& projectilePtr : projectilePtrs) {
		if (projectilePtr->lifetime-- <= 0) { removePtrs.push_back(projectilePtr); }
		projectilePtr->think(ui);
	}
	for (auto&& projectilePtr : removePtrs) {
		delete projectilePtr;
		projectilePtrs.remove(projectilePtr);
	}
	return p_update(target);
}

Projectile* Equipment::release() {
	auto r = p_release();
	this->target = Target();
	primed = false;
	return r;
}

void Equipment::draw() {
	if (userPtr) { sprites.front().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y); }
	for (auto&& projectilePtr : projectilePtrs) { projectilePtr->draw(); }
}

Projectile* ScanMarker::p_prime() {
	if (userPtr) { userPtr->statEffects.markPtr = nullptr; }
	return nullptr;
}

Projectile* ScanMarker::p_update(Target target) {
	if (primed && target.bodyPtr && this->target.bodyPtr == target.bodyPtr) { ++m_frames; }
	else { this->target = target; m_frames = 0; }
	return nullptr;
}

Projectile* ScanMarker::p_release() {
	if (m_frames > chargingTime) { userPtr->statEffects.markPtr = target.bodyPtr; }
	m_frames = 0;
	return nullptr;
}

void ScanMarker::draw() {
	if (userPtr && userPtr->statEffects.markPtr) {
		sprites.back().setPosition(
			glm::vec2(
				userPtr->statEffects.markPtr->b2Ptr->GetPosition().x,
				userPtr->statEffects.markPtr->b2Ptr->GetPosition().y
			)
		);
		float a = (255.0 / chargingTime) * (m_frames > chargingTime ? chargingTime : m_frames);
		sprites.back().setColor(255, 0, 0, 255);
	} else if (primed && target.bodyPtr) {
		sprites.back().setPosition(target.bodyPtr->b2Ptr->GetPosition().x, target.bodyPtr->b2Ptr->GetPosition().y);
		float a = (255.0 / chargingTime) * (m_frames > chargingTime ? chargingTime : m_frames);
		sprites.back().setColor(255, 0, 0, (char)a);
	} else {
		sprites.back().setColor(255, 0, 0, 0);
	}
	Equipment::draw();
}

Projectile* Retracer::p_prime() {
	target.location = glm::vec2(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
	return nullptr;
}

Projectile* Retracer::p_release() {
	if (userPtr) { userPtr->b2Ptr->SetTransform(b2Vec2(target.location.x, target.location.y), 0); }
	return nullptr;
}

void Retracer::draw() {
	if (primed) {
		sprites.back().getCanvasRef().color.a = 255;
		sprites.back().setPosition(target.location);
	} else {
		sprites.back().getCanvasRef().color.a = 0;
	}
	Equipment::draw();
}

Projectile* Booster::p_release() {
	if (userPtr) { userPtr->statEffects.speedBoost = 50.0; }
	return nullptr;
}

void Booster::draw() {
	if (primed && userPtr) {
		sprites.back().getCanvasRef().color.a = 255;
		auto b = userPtr->b2Ptr->GetPosition();
		sprites.back().setPosition(b.x, b.y);
	} else {
		sprites.back().getCanvasRef().color.a = 0;
	}
	Equipment::draw();
}

Projectile* SFCharge::p_prime() {
	m_frames = 0;
	m_triggered = false;
	return nullptr;
}

Projectile* SFCharge::p_update(Target target) {
	++m_frames;
	if (userPtr && m_frames > chargingTime && m_triggered) {
		auto pd = Projectile::Def(projectileSpawner);
		pd.size = 2;
		pd.bd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new SField(p_spriteBatch, userPtr->world, pd));
		return projectilePtrs.back();
		m_triggered = false;
		m_frames = 0;
	}
	return nullptr;
}

Projectile* SFCharge::p_release() {
	m_triggered = true;
	return nullptr;
}

void SFCharge::draw() {
	if (userPtr) {
		sprites.back().setPosition(target.location);
	}
	Equipment::draw();
}

Projectile* HealField::p_prime() {
	if (userPtr) {
		projectileSpawner.bd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new HField(p_spriteBatch, userPtr->world, projectileSpawner));
		return projectilePtrs.back();
	}
	return nullptr;
}

Projectile* DetectorDust::p_prime() {
	if (userPtr) {
		projectileSpawner.bd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new DDust(p_spriteBatch, userPtr->world, projectileSpawner));
		return projectilePtrs.back();
	}
	return nullptr;
}

Projectile* DetectorDust::p_update(Target target) {
	sprites.back().setPosition(userPtr->b2Ptr->GetPosition().x, userPtr->b2Ptr->GetPosition().y);
	return nullptr;
}

Projectile* CPC::p_prime() {
	if (userPtr) {
		projectileSpawner.bd.position = userPtr->b2Ptr->GetPosition();
		projectilePtrs.push_back(new CPField(p_spriteBatch, userPtr->world, projectileSpawner));
		return projectilePtrs.back();
	}
	return nullptr;
}

Projectile* Cloak::p_prime() {
	for (auto&& sprite : userPtr->sprites) { sprite.setColor(0,0,0,0); }
	for (auto&& sprite : sprites) { sprite.setColor(0, 0, 0, 0); }
	return nullptr;
}

Projectile* Cloak::p_release() {
	for (auto&& sprite : userPtr->sprites) { sprite.setColor(255, 255, 255, 255); }
	for (auto&& sprite : sprites) { sprite.setColor(255, 255, 255, 255); }
	return nullptr;
}

Projectile* Anchor::p_prime() {
	if (userPtr) {
		projectileSpawner.bd.position = userPtr->b2Ptr->GetPosition() + b2Vec2(.3, .3);
		projectilePtrs.push_back(new Chain(p_spriteBatch, userPtr->world, projectileSpawner));
		return projectilePtrs.back();
	}
	return nullptr;
}

Projectile* ShieldWall::p_prime() {
	return nullptr;
}

Projectile* ShieldWall::p_release() {
	return nullptr;
}

Projectile* ShieldWall::p_update(Target target) {
	return nullptr;
}

void ShieldWall::draw() {
	
}