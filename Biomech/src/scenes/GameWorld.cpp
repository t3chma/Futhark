#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>


void GameWorld::create(fk::Tools& tools) {
	std::vector<fk::Shader> shaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	textBatchPtr = new fk::SpriteBatch(true);
	spriteRenderer.setShaders(shaders);
	spritesPtr = new fk::Sprites(*textBatchPtr);
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(70);
	cam.setPosition(glm::vec2(250, 250));

	std::vector<fk::Texture> textures;
	textures.push_back(tools.textures.get("Selector.png", 1));
	spritesPtr->add("cursor", textures[0]);
	spritesPtr->get("cursor")->setDimensions(1.0, 1.0);

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
	spritesPtr->get("cursor")->setPosition(
		cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position)
	);
	if (tools.ui.getKeyInfo(fk::Key::D).downFrames > 1) { cam.move(glm::vec2(0.1,0)); }
	if (tools.ui.getKeyInfo(fk::Key::A).downFrames > 1) { cam.move(glm::vec2(-0.1, 0)); }
	if (tools.ui.getKeyInfo(fk::Key::W).downFrames > 1) { cam.move(glm::vec2(0, 0.1)); }
	if (tools.ui.getKeyInfo(fk::Key::S).downFrames > 1) { cam.move(glm::vec2(0, -0.1)); }
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto matrix = cam.getTransScaledMatrix();
	spriteRenderer.render(*textBatchPtr, matrix);
}
