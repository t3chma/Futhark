#pragma once
#include "Equipment.h"


class Weapon : public Equipment {
  public:
	Weapon(fk::SpriteBatch& sb, Def ed);
	~Weapon();
};


class Shotgun : public Weapon {
public:
	Shotgun(fk::SpriteBatch& sb, Def ed) : Weapon(sb, ed) {
		properties.projectileSpawner.physical.power = 2;
		properties.projectileSpawner.physical.durration = 1;
		properties.projectileSpawner.heat.power = 0;
		properties.projectileSpawner.heat.durration = 0;
		properties.projectileSpawner.corruption.power = 0;
		properties.projectileSpawner.corruption.durration = 0;
		properties.projectileSpawner.dropoff.start = 10;
		properties.projectileSpawner.dropoff.reduction = 0.2;
		properties.projectileSpawner.health = 0;
		properties.projectileSpawner.cost = 0;

		properties.delivery = 5;
		properties.bursting.size.full = 1;
		properties.bursting.delay.full = 2;
		properties.bursting.cooldown.full = 0;
		properties.charged.full = 0;
		properties.heatGeneration = 0;
		properties.aim.full = 3;
		properties.loading.full = 10;
		properties.loading.amount = 50;
		properties.clip.full = 50;

		properties.kick.power = fk::TAU / 16;
		properties.spread = fk::TAU / 128;
		properties.projectileSpawner.lifetime = fk::SECOND * 4;
		properties.gauge = 1;
		properties.projectileSpawner.size = 0.2;
		properties.projectileSpawner.splash.range = 0;
		properties.projectileSpawner.splash.innerDamage = 0;
		properties.projectileSpawner.splash.outerDamage = 0;
	};
};

class SMG : public Weapon {
public:
	SMG(fk::SpriteBatch& sb, Def ed) : Weapon(sb, ed) {
		properties.projectileSpawner.physical.power = 2;
		properties.projectileSpawner.physical.durration = 1;
		properties.projectileSpawner.heat.power = 0;
		properties.projectileSpawner.heat.durration = 0;
		properties.projectileSpawner.corruption.power = 0;
		properties.projectileSpawner.corruption.durration = 0;
		properties.projectileSpawner.dropoff.start = 5;
		properties.projectileSpawner.dropoff.reduction = 1;
		properties.projectileSpawner.health = 0;
		properties.projectileSpawner.cost = 0;

		properties.delivery = 5;
		properties.bursting.size.full = 1;
		properties.bursting.delay.full = 0;
		properties.bursting.cooldown.full = 40;
		properties.charged.full = 0;
		properties.heatGeneration = 0;
		properties.aim.full = 10;
		properties.loading.full = 30;
		properties.loading.amount = 1;
		properties.clip.full = 7;

		properties.kick.power = fk::TAU / 8;
		properties.spread = fk::TAU / 16;
		properties.projectileSpawner.lifetime = fk::SECOND * 4;
		properties.gauge = 12;
		properties.projectileSpawner.size = 0.2;
		properties.projectileSpawner.splash.range = 0;
		properties.projectileSpawner.splash.innerDamage = 0;
		properties.projectileSpawner.splash.outerDamage = 0;
	};
};