#pragma once
#include <vector>
#include <list>
#include "out/TTFont.h"
#include "actors/Player.h"


struct HudDef {
	Player* playerPtr{ nullptr };
	glm::ivec2 screenDimentions;
	fk::TTFont* fontPtr{ nullptr };
	struct {
		fk::Texture fire;
		fk::Texture water;
		fk::Texture earth;
		fk::Texture wind;
	} vials;
	fk::Texture dial;
	fk::Texture sterileBlood;
	fk::Texture castingBlood;
	fk::Texture itemSlot;
	fk::Texture cursor;
};

class Hud {
  public:
	struct Slot {
		Slot(fk::SpriteBatch& sb, fk::TTFont& f) : text(sb, f) {};
		fk::TextSprite text;
		int slotId;
		int itemId;
	};
	struct Bar {
		Bar(fk::SpriteBatch& sb, fk::TTFont& f) : text(sb, f) {};
		fk::TextSprite text;
		int vialId;
		int bloodId;
		int dialId;
		float baseBloodHeight;
		std::vector<int> castIds;
	};
	struct Bars {
		Bars(fk::SpriteBatch& sb, fk::TTFont& f) : fire(sb, f), water(sb, f), earth(sb, f), air(sb, f) {};
		Bar fire;
		Bar water;
		Bar earth;
		Bar air;
	};
	Player* playerPtr{ nullptr };
	std::vector<Slot> itemSlots;
	glm::ivec2 screenDimentions{ 0 };
	fk::TTFont& font;
	fk::SpriteBatch textBatch{ true };
	Bars bloodBars;
	fk::SpriteBatch frontBatch{ true };
	fk::SpriteBatch backBatch{ false };
	std::list<fk::TextSprite> statEffects;
	std::vector<fk::TextSprite> rightEnchants;
	std::vector<fk::TextSprite> leftEnchants;
	std::vector<fk::TextSprite> rightAmmo;
	std::vector<fk::TextSprite> leftAmmo;
	int cursorId{ 0 };
	Hud(HudDef hd);
	~Hud();
	void update();
};

