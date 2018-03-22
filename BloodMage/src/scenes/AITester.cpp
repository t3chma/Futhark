#include "AITester.h"
#include "base\Utility.h"
#include <math.h>


void AITester::create(fk::Tools& tools) {
	std::vector<fk::Shader> shaders{
		tools.shaders.get("Sprite.vert"),
		tools.shaders.get("Sprite.frag"),
		tools.shaders.get("Sprite.geom")
	};
	spriteRenderer.setShaders(shaders);
	cam.setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(40);
	tools.ui.setShowCursor(false);

	sprites.emplace_back(false);
	sprites.emplace_back(true);

	spriteIDs.push_back(sprites[0].makeSprite(tools.textures.get("Test.png", 1)));
	sprites[0][spriteIDs.back()].setColor(255, 255, 255, 50);
	for (int i = -100; i <= 100; i++) {
		for (int j = -100; j <= 100; j++) {
			spriteIDs.push_back(sprites[0].makeSprite(tools.textures.get("DevGray128.png", 1)));
			sprites[0][spriteIDs.back()].setPosition(i, j);
		}
	}
	ActorDef ad;
	ad.position.y = 20;
	ad.textures.push_back(tools.textures.get("Circle.png", 1));
	ad.textures.push_back(tools.textures.get("Swipe.png", 1));
	ad.textures.push_back(tools.textures.get("Swipe.png", 1));
	ad.textures.push_back(tools.textures.get("Test.png", 1));
	ad.position.y = 0;
	actorPtrs.push_back(new Player(&sprites[1], world, &tools.ui, ad));
	
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
	cam.setPosition(actorPtrs[0]->getPosition());
	camMovement.x = 0;
	camMovement.y = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (auto&& spriteBatch : sprites) { spriteRenderer.Render(spriteBatch, cam.getTransScaledMatrix()); }

	for (auto&& propPtr : propPtrs) { propPtr->update(&cam); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->think(actorPtrs, &cam); }
	for (auto&& propPtr : propPtrs) { propPtr->updateBody(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateBody(); }
	world.update(1.0f/60.0f, 8, 3);
	for (auto&& propPtr : propPtrs) { propPtr->updateSprite(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->updateSprite(); }
	auto& mpos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).position);
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames > 1) {
		if (tools.ui.getKeyInfo(fk::Key::Z).downFrames == 1) {
			ActorDef ad;
			ad.position = mpos;
			ad.textures.push_back(tools.textures.get("Circle.png", 1));
			ad.textures.push_back(tools.textures.get("Swipe.png", 1));
			ad.textures.push_back(tools.textures.get("Swipe.png", 1));
			actorPtrs.push_back(new Grunt(&sprites[1], world, ad));
		}
		if (tools.ui.getKeyInfo(fk::Key::X).downFrames == 1) {
			ActorDef ad;
			ad.position = mpos;
			ad.textures.push_back(tools.textures.get("Circle.png", 1));
			ad.textures.push_back(tools.textures.get("Stab.png", 1));
			actorPtrs.push_back(new Spear(&sprites[1], world, ad));
		}
		if (tools.ui.getKeyInfo(fk::Key::MOUSE_RIGHT).downFrames == 1) {
			PropDef pd;
			pd.position.x = 0.5f + int(mpos.x);
			pd.position.y = 0.5f + int(mpos.y);
			pd.texture = tools.textures.get("DevGray128.png", 1);
		}
		if (tools.ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) {
			PropDef pd;
			pd.position.x = round(mpos.x);
			pd.position.y = round(mpos.y);
			pd.texture = tools.textures.get("DevOrange128.png", 1);
			pd.bodyType = b2_staticBody;
			propPtrs.push_back(new Prop(&sprites[1], world, pd));
		}
	}
}