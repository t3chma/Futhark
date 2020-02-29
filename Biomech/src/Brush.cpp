#include "Brush.h"

Brush::Brush(fk::Camera& cam, fk::SpriteBatch& sb, fk::TextureCache& tc) : Image(sb), cam(cam), tile(sb, tc, 0, 0) {
	camPos = cam.getPosition();
	for (auto&& sprite : tile.sprites) { sprite.move(camPos); }
}


Brush::~Brush() {

}

void Brush::draw() {
	if (edit) {
		for (auto&& sprite : tile.sprites) { sprite.getCanvasRef().color.a = 255; }
		edit = false;
	} else {
		for (auto&& sprite : tile.sprites) { sprite.getCanvasRef().color.a = 0; }
	}
	for (auto&& sprite : tile.sprites) { sprite.move(cam.getPosition() - camPos); }
	tile.draw();
	camPos = cam.getPosition();
}
