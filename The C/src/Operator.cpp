#include "Operator.h"

void GunMan::use() {
	bullets.emplace_back()
}

void GunMan::update() {
	for (auto&& bullet : bullets) {
		++bullet.lifetime;
	}
}
