#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>
#include "../Actor.h"

void GameWorld::create(fk::Tools& tools) {
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
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(70);
	cam.setPosition(glm::vec2(250, 250));

	cursorPtr = new fk::Sprite(*textBatchPtr, tools.textures.get("Selector.png", 1));
	cursorPtr->setDimensions(0.5, 0.5);

	font = tools.fonts.get("eldermagic.ttf");

	fk::TextSprite ts = font.generateCharSprites("Test", *textBatchPtr, glm::vec2(1.0));
	ts.setPosition(glm::vec2(249, 250));

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}
void GameWorld::destroy(fk::Tools& tools) {

}
void GameWorld::open(fk::Tools& tools) {

}
void GameWorld::close(fk::Tools& tools) {

}
void GameWorld::update(fk::Tools& tools) {
	cam.update();
	auto matrix = cam.getTransScaledMatrix();
	wireRenderer.render(world, matrix);
	world.update(1.0f / 60.0f, 4, 2);
	cursorPtr->setPosition(
		cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition)
	);
	///for (auto&& actorPtr : actorPtrs) { actorPtr->update(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->draw(); }
	float speed = 0.1;
	if (tools.ui.getKeyInfo(fk::Key::SHIFT_L).downFrames > 1) { speed = 0.2; }
	if (tools.ui.getKeyInfo(fk::Key::D).downFrames > 1) { cam.move(glm::vec2(speed,0)); }
	if (tools.ui.getKeyInfo(fk::Key::A).downFrames > 1) { cam.move(glm::vec2(-speed, 0)); }
	if (tools.ui.getKeyInfo(fk::Key::W).downFrames > 1) { cam.move(glm::vec2(0, speed)); }
	if (tools.ui.getKeyInfo(fk::Key::S).downFrames > 1) { cam.move(glm::vec2(0, -speed)); }
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spriteRenderer.render(*textBatchPtr, matrix);
}
