#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>

void GameWorld::create(fk::Tools& tools) {
	tools.ui.setShowCursor(false);
	
	// Set up srite batch
	spriteBatchPtr = new fk::SpriteBatch(true);
	textBatchPtr = new fk::SpriteBatch(true);
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
	cam.setZoom(100);

	// Text
	font = tools.fonts.get("calibri.ttf");
	std::string text;
	text += "test";
	fk::TextSprite ts = font.generateCharSprites(text, *spriteBatchPtr, glm::vec2(0.5));

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}
void GameWorld::destroy(fk::Tools& tools) {

}
void GameWorld::open(fk::Tools& tools) {

}
void GameWorld::close(fk::Tools& tools) {

}
void GameWorld::update(fk::Tools& tools) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Update cam and render.
	//wireRenderer.render(world, matrix);
	cam.update();
	auto batrix = cam.getBaseMatrix();
	auto matrix = cam.getTransScaledMatrix();
	spriteRenderer.render(*spriteBatchPtr, matrix);
	spriteRenderer.render(*textBatchPtr, batrix);
	
	// Physics
	world.update(1.0f / 60.0f, 3, 2);
	
	// Mouse
	glm::vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
	// Debug
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames || tools.ui.getKeyInfo(fk::Key::CTRL_R).downFrames) {
		if (tools.ui.getKeyInfo(fk::Key::Q).downFrames == 1) {
			
		}
		if (tools.ui.getKeyInfo(fk::Key::E).downFrames == 1) {

		}
	}
}
