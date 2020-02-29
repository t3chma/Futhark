#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>

void GameWorld::create(fk::Tools& tools) {
	tools.ui.setShowCursor(true);
	
	// Set up srite batch
	spriteBatchPtr = new fk::SpriteBatch(true);
	textBatchPtr = new fk::SpriteBatch(true);
	brushPtr = new Brush(cam, *spriteBatchPtr, tools.textures);
	std::vector<fk::Shader> spriteShaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	spriteRenderer.setShaders(spriteShaders);
	std::vector<fk::Shader> wireShaders{
		tools.shaders.get("Wire.vert"),
		tools.shaders.get("Wire.frag")
	};
	wireRenderer.setShaders(wireShaders);
	
	// Adjust cam
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(50);

	// Text
	font = tools.fonts.get("calibri.ttf");

	maps.emplace_back(*spriteBatchPtr, tools.textures);
	maps.back().generate(32);
	for (auto&& tile : maps[0].tiles) { imagePtrs.push_back(&tile); }

	glClearColor(1, 1, 1, 0.1f);
}
void GameWorld::destroy(fk::Tools& tools) {

}
void GameWorld::open(fk::Tools& tools) {

}
void GameWorld::close(fk::Tools& tools) {

}
void GameWorld::update(fk::Tools& tools) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Move
	glm::vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
	if (tools.ui.getKeyInfo(fk::Key::W).downFrames) { cam.move(glm::vec2(0, 1)); }
	if (tools.ui.getKeyInfo(fk::Key::A).downFrames) { cam.move(glm::vec2(-1, 0)); }
	if (tools.ui.getKeyInfo(fk::Key::S).downFrames) { cam.move(glm::vec2(0, -1)); }
	if (tools.ui.getKeyInfo(fk::Key::D).downFrames) { cam.move(glm::vec2(1, 0)); }
	if (tools.ui.getKeyInfo(fk::Key::SPACE).downFrames == 1) {
		if (maps[0].day) {
			glClearColor(0, 0, 0, 0.1f);
			maps[0].day = false;
		} else {
			glClearColor(255, 255, 255, 0.1f);
			maps[0].day = true;
		}
	}
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames == 1) { maps[0].edit = !maps[0].edit; }
	if (maps[0].edit) {
		brushPtr->edit = true;
		if (tools.ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames) {
			currentTilePtr->climate = brushPtr->tile.climate;
		}
		if (tools.ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames) {
			delete currentTilePtr->units.buildingPtr;
			delete currentTilePtr->units.vehiclePtr;
			for (auto&& personPtr : currentTilePtr->units.peoplePtrs) { delete personPtr; }
			currentTilePtr->units = brushPtr->tile.units;
		}
	}

	// Update cam and render.
	//wireRenderer.render(world, matrix);
	cam.update();
	auto batrix = cam.getBaseMatrix();
	auto matrix = cam.getTransScaledMatrix();
	spriteRenderer.render(*spriteBatchPtr, matrix);
	spriteRenderer.render(*textBatchPtr, batrix);
	
	// Physics
	world.update(1.0f / 60.0f, 3, 2);
	
	// Redering and AI
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
	brushPtr->draw();
}
