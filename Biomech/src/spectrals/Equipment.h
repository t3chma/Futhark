#pragma once
#include "../actors/Projectile.h"


struct Target {
	glm::vec2 location{ 0 };
	Body* bodyPtr{ nullptr };
};

class Equipment : public Image {
  public:
	struct Def {
		fk::Texture body;
		fk::Texture cursorAccesory;
		Projectile::Def projectileSpawner;
	};
	Equipment(fk::SpriteBatch& sb, Equipment::Def& ed);
	~Equipment() { for (auto&& projectilePtr : projectilePtrs) { delete projectilePtr; } };
	virtual Projectile* prime(int option, Boid* userPtr, Target target) final;
	virtual Projectile* update(fk::UserInput& ui, Target target) final;
	virtual Projectile* release() final;
	virtual void draw() override;
	std::list<Projectile*> projectilePtrs;
	Projectile::Def projectileSpawner;
	int option{ 0 };
	int chargingTime{ static_cast<int>(fk::SECOND) * 1 };
	Boid* userPtr{ nullptr };
	Target target;
	bool gun{ false };
	bool primed{ false };
  protected:
	virtual Projectile* p_prime() { return nullptr; }
	virtual Projectile* p_update(Target target) { return nullptr; };
	virtual Projectile* p_release() { return nullptr; };
};

class ScanMarker : public Equipment {
  public:
	ScanMarker(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile * p_prime() override;
	virtual Projectile* p_update(Target target) override;
	virtual Projectile* p_release() override;
	virtual void draw() override;
  private:
	int m_frames{ 0 };
};

class Retracer : public Equipment {
  public:
	Retracer(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile* p_prime() override;
	virtual Projectile* p_release() override;
	virtual void draw() override;
};

class Booster : public Equipment {
  public:
	Booster(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile* p_release() override;
	virtual void draw() override;
};

class SFCharge : public Equipment {
  public:
	SFCharge(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile* p_prime() override;
	virtual Projectile* p_release() override;
	virtual Projectile* p_update(Target target) override;
	virtual void draw() override;
  private:
	int m_frames{ 0 };
	bool m_triggered{ false };
};

class Anchor : public Equipment {
  public:
	Anchor(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {
		projectileSpawner.size = 0.03;
	};
	virtual Projectile* p_prime() override;
};

class Cloak : public Equipment {
  public:
	Cloak(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile* p_prime() override;
	virtual Projectile* p_release() override;
};

class CPC : public Equipment {
  public:
	CPC(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {
		projectileSpawner.size = 0.03;
	};
	virtual Projectile* p_prime() override;
};

class DetectorDust : public Equipment {
  public:
	DetectorDust(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {
		projectileSpawner.size = 2;
	};
	virtual Projectile* p_prime() override;
	virtual Projectile* p_update(Target target) override;
};

class HealField : public Equipment {
  public:
	HealField(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {
		projectileSpawner.size = 2;
	};
	virtual Projectile* p_prime() override;
};

class ShieldWall : public Equipment {
  public:
	ShieldWall(fk::SpriteBatch& sb, Equipment::Def& ed) : Equipment(sb, ed) {};
	virtual Projectile* p_prime() override;
	virtual Projectile* p_release() override;
	virtual Projectile* p_update(Target target) override;
	virtual void draw() override;
};