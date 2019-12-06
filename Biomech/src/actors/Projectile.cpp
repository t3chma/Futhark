#include "Projectile.h"
#include "Boid.h"
#include "../spectrals/Equipment.h"


Projectile::Projectile(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd)
	: Body(world, pd), Image(sb), properties(pd)
{
	type = (int)Type::PLAYER;
	team = (int)Team::PLAYER;
	species = (int)Spec::PLAYER;
	addCircleLimb(pd.size);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(10);
	limbs.back().b2Ptr->SetRestitution(0);
	sprites.emplace_back(p_spriteBatch, pd.body);
	sprites.back().setDimensions(pd.size * 2, pd.size * 2);
}


Projectile::~Projectile() {

}

void Projectile::draw() {
	sprites.back().setPosition(b2Ptr->GetPosition().x, b2Ptr->GetPosition().y);
};

SField::SField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Projectile(sb, world, pd) {
	sprites.back().setColor(0, 0, 0, 100);
	limbs.back().category = "s";
}

void SField::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	contactPtr->SetEnabled(false);
	auto body = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (
		triggered
		&& static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s"
		&& !collisionFixturePtr->IsSensor()
	) {
		//TODO: fix this.
		static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData())->statEffects.timeDilation = .5;
	}
}

HField::HField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Projectile(sb, world, pd) {
	sprites.back().setColor(0,255,0,100);
	limbs.back().category = "s";
}

void HField::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	contactPtr->SetEnabled(false);
	auto body = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s" && !collisionFixturePtr->IsSensor()) {
		//TODO: fix this.
		body->statEffects.healRate = 0.1;
		body->statEffects.healing = properties.lifetime + fk::SECOND * 1;
	}
}


CPField::CPField(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Projectile(sb, world, pd) {
	sprites.back().setColor(255, 255, 255, 100);
	limbs.back().category = "s";
}

void CPField::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	contactPtr->SetEnabled(false);
	auto body = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s" && !collisionFixturePtr->IsSensor()) {
		//TODO: fix this.
		static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData())->statEffects.blind =
			properties.lifetime + fk::SECOND * 1;
	}
}

DDust::DDust(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Projectile(sb, world, pd) {
	properties.makerPtr->sprites.back().setColor(0, 0, 0, 0);
	sprites.back().setColor(128, 0, 0, 1);
}

void DDust::think(fk::UserInput& ui) {
	properties.makerPtr->sprites.back().setColor(0, 0, 0, 0);
	sprites.back().setColor(128, 0, 0, 1);
};

void DDust::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	contactPtr->SetEnabled(false);
	auto body = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s" && !collisionFixturePtr->IsSensor()) {
		//TODO: fix this.
		sprites.back().setColor(128, 0, 0, 2);
		properties.makerPtr->sprites.back().setColor(255, 0, 0, 128);
	}
}

Chain::Chain(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Projectile(sb, world, pd) {
	properties.makerPtr->sprites.back().setColor(0, 0, 0, 0);
}

void Chain::think(fk::UserInput& ui) {
	properties.makerPtr->sprites.back().setColor(0, 0, 0, 0);
};

void Chain::p_preCollisionAdjusting(
	b2Fixture* collisionFixturePtr,
	b2Fixture* myFixturePtr,
	b2Contact* contactPtr,
	const b2Manifold* oldManifoldPtr
) {
	contactPtr->SetEnabled(false);
	auto body = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (static_cast<Limb*>(collisionFixturePtr->GetUserData())->category != "s" && !collisionFixturePtr->IsSensor()) {
		auto victimPtr = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
		properties.lifetime = 0;
		victimPtr->statEffects.chained = fk::SECOND * 3;
		victimPtr->statEffects.chainedLocation = victimPtr->b2Ptr->GetPosition();
		properties.makerPtr->sprites.back().setPosition(
			glm::vec2(victimPtr->statEffects.chainedLocation.x, victimPtr->statEffects.chainedLocation.y)
		);
		properties.makerPtr->sprites.back().setColor(0, 0, 0, 255);
	}
}