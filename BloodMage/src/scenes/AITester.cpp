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
	spriteIDs.emplace_back(sprites.makeSprite(tools.textures.get("Test.png", 1)));
	sprites[spriteIDs.back()].canvas.dimensions = glm::vec2(100);
}
void AITester::destroy(fk::Tools& tools) {

}
void AITester::open(fk::Tools& tools) {

}
void AITester::close(fk::Tools& tools) {

}
void AITester::update(fk::Tools& tools) {
	cam.update();
	if (tools.ui.getKeyInfo(fk::Key::W).downFrames > 1) {
		camMovement.y += 1;
	}
	if (tools.ui.getKeyInfo(fk::Key::S).downFrames > 1) {
		camMovement.y -= 1;
	}
	if (tools.ui.getKeyInfo(fk::Key::D).downFrames > 1) {
		camMovement.x += 1;
	}
	if (tools.ui.getKeyInfo(fk::Key::A).downFrames > 1) {
		camMovement.x -= 1;
	}
	if (camMovement.x || camMovement.y) cam.move(glm::vec2(10) * glm::normalize(camMovement));
	camMovement.x = 0;
	camMovement.y = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spriteRenderer.Render(sprites, cam.getTransScaledMatrix());
	auto& mpos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position);
	if (!spriteIDs.empty()) {
		sprites[spriteIDs.back()].canvas.rotationAngle -= 0.1;
		sprites[spriteIDs.back()].setPosition(mpos);
		sprites[spriteIDs.back()].canvas.rotationAxis = mpos;
	}
}