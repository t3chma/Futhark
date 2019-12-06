#pragma once
#include "../actors/Projectile.h"


struct Target {
	glm::vec2 location{ 0 };
	Body* bodyPtr{ nullptr };
};

class Equipment : public Image {
  public:
	struct Properties {
		// How fast a bullet is launched
		float delivery{ 5 };
		// Firing/bursting timers
		struct {
			// Size of burst
			struct {
				int full{ 1 };
				int current{ 0 };
			} size;
			// Firing delay
			struct {
				int full{ 1 };
				int current{ 0 };
			} delay;
			// Burst cooldown
			struct {
				int full { 0 };
				int current{ 0 };
			} cooldown;
			operator bool() { return size.current; };
		} bursting;
		// Charging timer
		struct {
			int full{ 0 };
			int current{ 0 };
			operator bool() { return current >= full; };
		} charged;
		// Is a bullet launch triggered
		int triggered{ false };
		// Is the priming button help down
		bool primed{ false };
		// Heat generation for use
		int heatGeneration{ 0 };
		// Aiming timer
		struct {
			int full{ 3 }; //
			int current{ 0 }; //
			operator bool() { return current >= 0; };
		} aim;
		// Loading timer and info
		struct {
			int full{ 30 };
			int current{ 0 };
			// How much to reload
			int amount{ 0 };
			operator bool() { return current > 0; };
		} loading;
		// Clip data
		struct {
			int full{ 10 };
			int current{ -1 };
			operator bool() { return current != 0; };
		} clip;
		// Kickback meter
		struct {
			double power{ fk::TAU / 64 };
			double current{ 0 };
		} kick;
		// Fire spread
		double spread{ fk::TAU / 64 };
		// Weapon guage
		int gauge{ 1 };
		// Projectile info
		Projectile::Def projectileSpawner;
	} properties;
	// Used for kickback/spread calculations
	fk::Random rangen;
	struct Def : public Properties {
		fk::Texture body;
		fk::Texture cursorAccesory;
	};
	Equipment(fk::SpriteBatch& sb, Def ed);
	~Equipment() { for (auto&& projectilePtr : projectilePtrs) { delete projectilePtr; } };
	virtual void prime(int option, Boid* userPtr, Target target) final;
	virtual Projectile* update(fk::UserInput& ui, Target target) final;
	virtual void release() final;
	virtual void reload() final;
	virtual void draw() final override;
	// List of projectiles made by this class
	std::list<Projectile*> projectilePtrs;
	// What option is this set to
	int option{ 0 };
	// Pointer to the equipment user
	Boid* userPtr{ nullptr };
	// Current target
	Target target;
	// If this is a gun
	bool gun{ false };
  protected:
	virtual void p_prime() {}
	virtual Projectile* p_update(Target target) { return nullptr; };
	virtual void p_release() {};
	virtual void p_draw() {};
};

class ScanMarker : public Equipment {
  public:
	ScanMarker(fk::SpriteBatch& sb, Def ed)
		: Equipment(sb, ed) {};
	virtual void p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_release() override;
	virtual void p_draw() override;
};

class Retracer : public Equipment {
  public:
	Retracer(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {};
	virtual void p_prime() override;
	virtual void p_release() override;
	virtual void p_draw() override;
};

class Booster : public Equipment {
  public:
	Booster(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {};
	virtual void p_prime() override;
	virtual void p_release() override;
	virtual void p_draw() override;
};

class SFCharge : public Equipment {
  public:

	SFCharge(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
		properties.charged.full = 60;
		properties.projectileSpawner.size = 2;
		properties.projectileSpawner.lifetime = fk::SECOND * 10;
	};
	virtual void p_prime() override;
	virtual void p_release() override;
	virtual Projectile* p_update(Target target) override;
};

class Anchor : public Equipment {
  public:
	Anchor(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
		properties.projectileSpawner.size = 0.03;
	};
	virtual void p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_release() override;
};

class Cloak : public Equipment {
  public:
	Cloak(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {};
	virtual void p_prime() override;
	virtual void p_release() override;
};

class CPC : public Equipment {
  public:
	CPC(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
		properties.projectileSpawner.size = 2;
		properties.projectileSpawner.lifetime = fk::SECOND * 10;
	};
	virtual void p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_release() override;
};

class DetectorDust : public Equipment {
  public:
	DetectorDust(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
		properties.projectileSpawner.size = 2;
		properties.projectileSpawner.lifetime = fk::SECOND * 10;
	};
	virtual void p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_release() override;
};

class HealField : public Equipment {
  public:
	HealField(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
		properties.projectileSpawner.size = 2;
	};
	virtual void p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_release() override;
};

class ShieldWall : public Equipment {
  public:
	ShieldWall(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {};
	virtual void p_prime() override;
	virtual void p_release() override;
	virtual Projectile* p_update(Target target) override;
	virtual void p_draw() override;
};