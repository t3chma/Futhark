#pragma once
#include "Boat.h"


class Player : public Boat {
  public:
	struct Def : public Boat::Def {
		Def() = delete;
		Def(
			fk::World& world, fk::TextureCache& textureCache, fk::SpriteBatch& batch,
			std::string boatFile, fk::UserInput& ui
		) :
			Boat::Def(world, textureCache, batch, boatFile)
		{
			uiPtr = &ui;
		}
	};
	Player(Def& pd);
	~Player();
	void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr);
	void p_beginCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
	void p_endCollision(
		b2Fixture* collisionFixturePtr,
		b2Fixture* myFixturePtr,
		b2Contact* contactPtr
	) override;
  private:
	struct M_Control : public State {
		M_Control(Actor& actor) : State(actor) {};
		virtual void enter() override;
		virtual void think(std::vector<Actor*>& actorPtrs, fk::Camera* camPtr = nullptr) override;
	};
	glm::vec2 m_mousePos;
	bool m_moused{ false };
};