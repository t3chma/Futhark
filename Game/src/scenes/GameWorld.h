#pragma once
#include "base/Scene.h"
#include "out/SpriteBatch.h"
#include "out/Camera.h"
#include "Box2D/Box2D.h"
#include "out/TTFont.h"
#include "out/SpriteRenderer.h"
#include "out/WireRenderer.h"
#include "../Body.h"
#include "../actors/Player.h"
#include "../Arena.h"
#include <iostream>
#include <map>

class Icon : public Image {
  public:
	Icon(fk::SpriteBatch& sb, fk::Texture t);
	virtual void draw() {};
};

class GameWorld : public fk::Scene {
  public:
	Arena::Move* currentCamFramePtr{ nullptr };
	int currentFrame{ 0 };
	char newBlock;
	std::vector<char*> charPtrs;
	fk::TextSprite* selectionInfoPtr{ nullptr };
	bool rEdit{ false };
	bool cEdit{ false };
	bool tEdit{ false };
	bool xEdit{ false };
	bool yEdit{ false };
	bool wEdit{ false };
	std::string input;
	Icon* editSelectionIconPtr{ nullptr };
	Icon* editSelection2IconPtr{ nullptr };
	Icon* editModeIconPtr{ nullptr };
	Mouse* editorMousePtr{ nullptr };
	std::list<fk::TextSprite> levelSelect;
	int currentLevel{ 0 };
	char mode{ 'p' };
	std::vector<std::string> levels;
	Arena* arenaPtr;
	std::vector<std::vector<TextBlock>> map;
	fk::World world;
	fk::SpriteBatch* spriteBatchPtr{ nullptr };
	fk::SpriteBatch* textBatchPtr{ nullptr };
	fk::Sprite* backgroundPtr;
	fk::SpriteRenderer spriteRenderer;
	fk::WireRenderer wireRenderer;
	fk::TTFont font;
	fk::Camera cam;
	std::vector<Intellect*> actorPtrs;
	std::vector<Image*> imagePtrs;
	Player* playerPtr;
	Player* player2Ptr;
	GameWorld() {};
	~GameWorld() {};
	// Inherited via fk::Scene
	virtual void create(fk::Tools& tools) override;
	virtual void destroy(fk::Tools& tools) override;
	virtual void open(fk::Tools& tools) override;
	virtual void close(fk::Tools& tools) override;
	virtual void update(fk::Tools& tools) override;

	void updateEdit(fk::Tools & tools, const glm::ivec2 &blockIndex);

	void writeInput(fk::Tools & tools, int & write);

	void writeChoregraphy();

	void reloadEdit(fk::Tools & tools);

	void updatePause(fk::Tools & tools);

};