#include "AITester.h"
#include "base\Utility.h"

void AITester::create(fk::Tools& tools) {
	std::vector<fk::Shader> shaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	spriteRenderer.setShaders(shaders);
	cam.setDimensions(tools.windowPtr->getDimensions());
	tools.ui.setShowCursor(false);
	camUp.camMovementPtr = &camMovement;
	tools.ui.bind(fk::Trigger::HOLD, fk::Key::W, &camUp);
	camDown.camMovementPtr = &camMovement;
	tools.ui.bind(fk::Trigger::HOLD, fk::Key::S, &camDown);
	camLeft.camMovementPtr = &camMovement;
	tools.ui.bind(fk::Trigger::HOLD, fk::Key::D, &camLeft);
	camRight.camMovementPtr = &camMovement;
	tools.ui.bind(fk::Trigger::HOLD, fk::Key::A, &camRight);
	localSprites.push_back(sprites.makeSprite(tools.textures.get("Test.png")));
	localSprites.back().getCanvasRef().dimensions = glm::vec2(100);
	localSprites.push_back(sprites.makeSprite(tools.textures.get("Test.png")));
	localSprites.back().getCanvasRef().dimensions = glm::vec2(100);
	//sprites.killSprite(s0);
}

void AITester::destroy(fk::Tools& tools) {

}

void AITester::open(fk::Tools& tools) {

}

void AITester::close(fk::Tools& tools) {

}

void AITester::update(fk::Tools& tools) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spriteRenderer.Render(sprites, cam.getTransScaledMatrix());
	localSprites.back().getCanvasRef().rotationAngle -= 0.1;
	auto& mpos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position);
	auto& cpos = localSprites.back().getCanvasRef().position;
	auto& caxis = localSprites.back().getCanvasRef().rotationAxis;
	cpos.x = mpos.x;
	cpos.y = mpos.y;
	caxis = mpos;
	cam.update();
	if (camMovement.x || camMovement.y) cam.move(glm::vec2(10) * glm::normalize(camMovement));
	camMovement.x = 0;
	camMovement.y = 0;
}
