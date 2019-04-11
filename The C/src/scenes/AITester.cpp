#include "AITester.h"
#include "base\Utility.h"
#include <iostream>
#include "../actors/States.h"


void AITester::create(fk::Tools& tools) {
	std::vector<fk::Shader> shaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	spriteRenderer.setShaders(shaders);
	std::vector<fk::Shader> wireShaders{
		tools.shaders.get("Wire.vert"),
		tools.shaders.get("Wire.frag")
	};
	wireRenderer.setShaders(wireShaders);
	textBatchPtr = new fk::SpriteBatch(true);
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(32);

	Player::Def pd(world, tools.textures, *textBatchPtr, "Basic.boat", tools.ui);
	pd.position.x = 0;
	pd.position.y = 0;
	playerPtr = new Player(pd);

	mousePtr = new Mouse(*textBatchPtr, tools.textures.get("Wave.png", 1), world);
	glClearColor(0.3f, 0.8f, 0.9f, 1.0f);
}
void AITester::destroy(fk::Tools& tools) {

}
void AITester::open(fk::Tools& tools) {
	
}
void AITester::close(fk::Tools& tools) {

}
void AITester::update(fk::Tools& tools) {
	cam.update();
	cam.setPosition(playerPtr->getPosition());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spriteRenderer.render(*textBatchPtr, cam.getTransScaledMatrix());
	auto matrix = cam.getTransScaledMatrix();
	//wireRenderer.render(world, matrix);
	mousePtr->position = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
	mousePtr->updateBody();
	mousePtr->updateSprites();
	playerPtr->think(actorPtrs, &cam);
	playerPtr->updateBody();
	playerPtr->updateSprites();
	if (tools.ui.getKeyInfo(fk::Key::APOSTROPHE).downFrames == 1) { getCommandLine(tools); }
	world.update(1.0f / 60.0f, 4, 2);
}

void AITester::getCommandLine(fk::Tools& tools) {
}
