#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>
#include "../actors/Dummy.h"

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
	text += "# Keys = Set current squad\n";
	text += "Ctrl + Middle Click = Activate/deactivate current squad\n";
	text += "Ctrl + Left Click = Add position to current squad\n";
	text += "Ctrl + Right Click = Remove position from current squad\n";
	text += "Ctrl + q = Add dummy\n";
	text += "Ctrl + e = Add enemy\n";
	text += "Shift + Right Click = Set primary objective for current squad\n";
	text += "Shift + Left Click = Set primary objective for current squad\n";
	text += "WASD = Move chara";
	fk::TextSprite ts = font.generateCharSprites(text, *spriteBatchPtr, glm::vec2(0.5));

	// Player
	Player::Def pd;
	pd.body = tools.textures.get("Circle.png", 1);
	pd.md.body = tools.textures.get("Circle.png", 1);
	pd.botSpawner.body = tools.textures.get("Circle.png", 1);
	pd.hudFont = tools.fonts.get("calibri.ttf");
	playerPtr = new Player(*spriteBatchPtr, *textBatchPtr, world, pd);
	actorPtrs.push_back(playerPtr);
	imagePtrs.push_back(playerPtr);

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
	cam.setPosition(glm::vec2(playerPtr->b2Ptr->GetPosition().x, playerPtr->b2Ptr->GetPosition().y));
	cam.update();
	auto batrix = cam.getBaseMatrix();
	auto matrix = cam.getTransScaledMatrix();
	spriteRenderer.render(*spriteBatchPtr, matrix);
	spriteRenderer.render(*textBatchPtr, batrix);
	
	// Physics
	world.update(1.0f / 60.0f, 3, 2);
	
	// Mouse
	glm::vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
	playerPtr->mouse.b2Ptr->SetTransform(b2Vec2(mousePos.x, mousePos.y), 0);
	// Debug
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames || tools.ui.getKeyInfo(fk::Key::CTRL_R).downFrames) {
		if (tools.ui.getKeyInfo(fk::Key::Q).downFrames == 1) {
			Dummy::Def dd;
			dd.body = tools.textures.get("Circle.png", 1);
			dd.bd.sightRadius = 1;
			dd.bd.bd.position = fk::Vec2(mousePos.x, mousePos.y);
			auto dummy = new Dummy(*spriteBatchPtr, world, dd);
			actorPtrs.push_back(dummy);
			imagePtrs.push_back(dummy);
		}
		if (tools.ui.getKeyInfo(fk::Key::E).downFrames == 1) {
			Dummy::Def dd;
			dd.body = tools.textures.get("Circle.png", 1);
			dd.bd.sightRadius = 1;
			dd.bd.bd.position = fk::Vec2(mousePos.x, mousePos.y);
			auto dummy = new DumRunner(*spriteBatchPtr, world, dd);
			actorPtrs.push_back(dummy);
			imagePtrs.push_back(dummy);
		}
	}
	
	// Redering and AI
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
	for (auto&& actorPtr : actorPtrs) { actorPtr->update(tools.ui); }
}
