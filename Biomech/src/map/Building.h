#pragma once
#include "../Stats.h"
#include <string>
#include "out/SpriteBatch.h"
#include "out/TTFont.h"
#include "../Image.h"
#include "base/Tools.h"
#include "../actors/Person.h"
#include "../Operation.h"

class Building : public StatEffects, Image {
  public:
	enum Type {
		NONE = 0, CAPITOL,
		INTEL, UNIVERSITY, HOSPITAL, TRADE, STATE, TRAINING,
		RADIO, TRAVEL, SHIELD, LIBRARY, TURRET,
		UTILITY, LIGHT, HEAVY, COMBAT, WAREHOUSE, FORUM, BANK,
		FARM, SUBURB, SETTLEMENT, COUNTRY, OUTPOST, CABIN
	} type{ Type::NONE };
	Building(fk::TextureCache& t, fk::SpriteBatch& sb, fk::TTFont& f) : Image(sb), textures(t), op(t) {
		sprites.emplace_back(sb, fk::Texture(0));
		sprites.back().getCanvasRef().position.z = 1;
		peopleTextPtr = new fk::TextSprite(sb, f);
		healthTextPtr = new fk::TextSprite(sb, f);
	}
	fk::TextureCache& textures;
	void setType(Type t);
	void draw() override;
	enum class Placement { DEV, CAPITAL, STATE, OUTSKIRT } placement{ Placement::DEV };
	enum class Recidency { NONE, HOST, HOME, WILD } usage{ Recidency::NONE };
	Operation op;
	int people{ 0 };
	fk::TextSprite* peopleTextPtr;
	float health{ 10 };
	fk::TextSprite* healthTextPtr;
	~Building();
};