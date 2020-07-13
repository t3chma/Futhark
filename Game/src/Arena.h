#pragma once
#include "actors/TextBlock.h"
#include "actors/Player.h"

class Arena {
  public:
	std::vector<std::string> buffer;
	std::list<std::list<TextBlock>> map;
	Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def& pd);
	Arena(std::vector<std::string>& buffer, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w, Player::Def& pd);
	~Arena();
	void init(fk::TTFont& f, fk::SpriteBatch& sb, fk::World& world, fk::Texture& t, Player::Def& pd);
	fk::Vec2 fileToMap(float lineIndex, float charIndex);
	fk::Vec2 mapToFile(fk::Vec2 gridPos);
	void draw();
	void update(fk::UserInput & ui);
	char* getCharAt(glm::ivec2 levelIndex);
	std::vector<fk::Vec2> spawns;
	std::vector<fk::Vec2> allSpawns;
	class Move : public Image {
	  public:
		Move(fk::SpriteBatch& sb) : Image(sb) {}
		bool operator == (const Move& rhs) {
			return this == &rhs;
		}
		void draw() {};
		int t{ 60 };
		fk::Vec2 position;
	};
	class Wind : public Image {
	  public:
		Wind(fk::SpriteBatch& sb) : Image(sb) {}
		void draw() {};
		int t{ 0 };
		fk::Vec2 d;
	};
	std::list<Player> bots;
	std::list<Move> choreography;
	int c{ 0 };
	std::list<Wind> weather;
	int w{ 0 };
	fk::Vec2 cam;
	fk::Vec2 gravity;
	fk::Vec2 start;
	bool startPos{ false };
	int cTime{ 0 };
	int wTime{ 0 };
	float width{ 0 };
	bool freezeCam{ false };
	int maxSize{ 1024 };
	fk::Vec2 step;
	fk::Vec2 old;
	int metadata{ 2 };
};

