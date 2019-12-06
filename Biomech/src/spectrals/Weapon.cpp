#include "Weapon.h"


Weapon::Weapon(fk::SpriteBatch& sb, Def ed) : Equipment(sb, ed) {
	gun = true;
}

 
Weapon::~Weapon() {
}
