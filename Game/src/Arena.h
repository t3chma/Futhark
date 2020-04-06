#pragma once
#include "actors/TextBlock.h"
#include "actors/Player.h"

class Arena {
  public:
	std::vector<std::string> buffer;
	std::list<std::list<TextBlock>> map;
	Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def pd);
	Arena(std::vector<std::string>& buffer, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def pd);
	~Arena();
	void init(fk::TTFont & f, fk::SpriteBatch & sb, fk::World & world, fk::Texture & t);
	void draw();
	void update(fk::UserInput & ui);
	char* getCharAt(glm::ivec2 levelIndex);
	std::vector<b2Vec2> spawns;
	struct Move {
		int t{ 0 };
		glm::vec2 step;
	};
	struct Wind {
		glm::vec2 d;
		int t{ 0 };
	};
	std::vector<Move> choreography;
	int c{ 0 };
	std::vector<Wind> weather;
	int w{ 0 };
	glm::vec2 cam;
	glm::vec2 gravity;
	int ctime{ 0 };
	int wtime{ 0 };
	float width{ 0 };
	bool freezeCam{ false };
	int maxSize{ 1024 };
};

