#pragma once
#include "Boid.h"
#include <functional>


class Bot : public Boid {
public:
	struct Def {
		Boid::Def db;
		fk::Texture body;
		Body* basePtr{ nullptr };
	};

	Bot(fk::SpriteBatch& sb, fk::World& world, Def& bd);
	~Bot();
	virtual void p_think(fk::UserInput& ui) override;
	void draw() override;
};

struct HiveMind {
	struct Squad {
		bool active{ true };
		std::list<Bot> bots;
	};
	size_t squadSize();
	bool isActive();
	void toggleActivation();
	void removeBot();
	void addBot(fk::SpriteBatch& sb, fk::World& w, Bot::Def bd);
	void setPrimaryGoal(glm::vec2& location, Body* bodyPtr = nullptr);
	void setSecondaryGoal(glm::vec2& location, Body* bodyPtr = nullptr);
	void launchSquad(glm::vec2& target, glm::vec2& launcher);
	std::vector<Squad> squads;
	int squad{ 1 };
	HiveMind();
};
