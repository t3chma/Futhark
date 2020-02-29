#include "Climate.h"

void Climate::Biome::setType(Type t) {
	type = t;
	fk::Texture tex(0);
	sprites.back().setColor(255, 255, 255, 255);
	reset();
	switch (t) {
	case TREES:
		los = 1;
		armorP = 1;
		armorV = 1;
		visibilityP = -1;
		visibilityV = -1;
		impedimentV = 1;
		tex = textures.get("climate/trees.png", 1);
		break;
	case DESERT:
		visibilityP = 1;
		visibilityV = 1;
		mood = -1;
		tex = textures.get("climate/desert.png", 1);
		break;
	case ICE:
		impedimentP = 1;
		impedimentV = 1;
		mood = -1;
		tex = textures.get("climate/ice.png", 1);
		break;
	case FLOWERS:
		mood = 1;
		tex = textures.get("climate/flowers.png", 1);
		break;
	case RUINS:
		foundable = true;
		los = 1;
		armorP = 1;
		armorV = 1;
		visibilityP = -1;
		visibilityV = -1;
		tex = textures.get("climate/ruins.png", 1);
		break;
	case RIVER:
		impedimentP = 1;
		impedimentV = 1;
		tex = textures.get("climate/river.png", 1);
		break;
	case GOOP:
		foundable = true;
		mood = -1;
		tex = textures.get("climate/goop.png", 1);
		break;
	default:
		sprites.back().setColor(255, 255, 255, 0);
		break;
	}
	sprites.back().setTexture(tex);
}

void Climate::Biome::draw()
{
}

void Climate::Ground::setType(int t) {
	type = t;
	fk::Texture tex(0);
	sprites.back().setColor(255, 255, 255, 255);
	reset();
	switch (t) {
	case MINES:
		foundable = true;
		armorP = 2;
		visibilityP = -2;
		tex = textures.get("climate/mines.png", 1);
		break;
	case CRATER:
		foundable = true;
		visibilityP = -2;
		visibilityV = -2;
		sight = -1;
		impedimentP = 1;
		impedimentV = 1;
		tex = textures.get("climate/crater.png", 1);
		break;
	case HILL:
		los = 1;
		sight = 1;
		visibilityP = 1;
		visibilityV = 1;
		impedimentP = 1;
		impedimentV = 1;
		tex = textures.get("climate/hill.png", 1);
		break;
	case MOUNTAIN:
		los = 2;
		sight = 2;
		visibilityP = 1;
		visibilityV = 1;
		impedimentP = 2;
		impedimentV = 2;
		tex = textures.get("climate/mountain.png", 1);
		break;
	default:
		sprites.back().setColor(255, 255, 255, 0);
		break;
	}
	sprites.back().setTexture(tex);
}

void Climate::Ground::draw()
{
}
