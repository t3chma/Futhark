#include "Projectile.h"


Projectile::Projectile(fk::SpriteBatch& sb, fk::World& world, Projectile::Def pd) : Body(world, pd.bd), Image(sb) {
	type = (int)Type::PLAYER;
	team = (int)Team::PLAYER;
	species = (int)Spec::PLAYER;
	addCircleLimb(0.1);
	limbs.back().category = "body";
	limbs.back().b2Ptr->SetDensity(10);
	limbs.back().b2Ptr->SetRestitution(0);
	p_sprites.emplace_back(p_spriteBatch, pd.body);
}


Projectile::~Projectile() {

}

void Projectile::draw() {

};

void Projectile::update(fk::UserInput& ui) {

};