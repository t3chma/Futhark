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
	cam.setZoom(64);
	tools.ui.setShowCursor(false);

	spriteIDs.push_back(sprites.makeSprite(tools.textures.get("Test.png", 1)));
	sprites[spriteIDs.back()].setDepth(-1);
	fk::ActorDef ad;
	ad.textures.push_back(tools.textures.get("Circle.png", 1));
	actorPtrs.push_back(new Grunt(&sprites, &world, ad));
	ad.textures.push_back(tools.textures.get("Test.png", 1));
	ad.textures.push_back(tools.textures.get("Slash.png", 1));
	ad.textures.push_back(tools.textures.get("Slash.png", 1));
	actorPtrs.push_back(new Player(&sprites, &world, &tools.ui, ad));
}
void AITester::destroy(fk::Tools& tools) {

}
void AITester::open(fk::Tools& tools) {

}
void AITester::close(fk::Tools& tools) {

}
void AITester::update(fk::Tools& tools) {
	cam.update();
	if (tools.ui.getKeyInfo(fk::Key::Q).downFrames > 1) { cam.zoomOut(); }
	if (tools.ui.getKeyInfo(fk::Key::E).downFrames > 1) { cam.zoomIn(); }
	//cam.setPosition(glm::vec2(0));
	cam.setPosition(actorPtrs.back()->getPosition());
	camMovement.x = 0;
	camMovement.y = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spriteRenderer.Render(sprites, cam.getTransScaledMatrix());

	for (auto&& actorPtr : actorPtrs) { actorPtr->think(actorPtrs, &cam); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateBody(); }
	world.Step(1.0f/60.0f, 8, 3);
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateSprite(); }
	auto& mpos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position);

	//sprites[spriteIDs[0]].setPosition(bodyPtrs[0]->GetPosition().x, bodyPtrs[0]->GetPosition().y);
	//sprites[spriteIDs[0]].setRotationAxis(bodyPtrs[0]->GetPosition().x, bodyPtrs[0]->GetPosition().y);
	//sprites[spriteIDs[0]].canvas.rotationAngle = bodyPtrs[0]->GetAngle();
}