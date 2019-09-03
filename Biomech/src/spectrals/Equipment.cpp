#include "Equipment.h"



Equipment::Equipment(fk::SpriteBatch& sb, Equipment::Def ed) : Image(sb) {
	p_sprites.emplace_back(p_spriteBatch, ed.body);
}


Equipment::~Equipment() {

}

void Equipment::draw() {

}

void Equipment::update(fk::UserInput & ui) {

}
