#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>
#include "../actors/Dummy.h"
#include "../spectrals/Equipment.h"

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
	text += "Ctrl + 1 = Scan Marker\n";
	text += "Ctrl + 2 = Retracer\n";
	text += "Ctrl + 3 = Booster\n";
	text += "Ctrl + 4 = Slow Field Charge\n";
	text += "Ctrl + 5 = Anchor\n";
	text += "Ctrl + 6 = Cloak\n";
	text += "Ctrl + 7 = Cold Plasma Cloud\n";
	text += "Ctrl + 8 = Detector Dust\n";
	text += "Ctrl + 9 = Heal Field\n";
	text += "Ctrl + 0 = Broken\n";
	text += "z = Add dummy\n";
	text += "x = Add enemy\n";
	text += "q = Make squad 1 the current squad\n";
	text += "e = Make squad 2 the current squad\n";
	text += "Ctrl + Middle Click = Activate/deactivate current squad\n";
	text += "Ctrl + Left Click = Add position to current squad\n";
	text += "Ctrl + Right Click = Remove position from current squad\n";
	text += "Right Click = Set secondary objective for current squad\n";
	text += "Left Click = Set primary objective for current squad\n";
	text += "Shift = Toggle aim\n";
	text += "Mouse Wheel = zoom\n";
	text += "WASD = Move chara";
	fk::TextSprite ts = font.generateCharSprites(text, *spriteBatchPtr, glm::vec2(0.5));

	// Player
	Player::Def pd;
	pd.body = tools.textures.get("circle128.png", 1);
	pd.md.body = tools.textures.get("circle128.png", 1);
	pd.botSpawner.body = tools.textures.get("circle128.png", 1);
	pd.position.x = 0;
	pd.position.y = 0;
	playerPtr = new Player(*spriteBatchPtr, *textBatchPtr, tools.fonts.get("calibri.ttf"), cam, world, pd);
	aiPtrs.push_back(playerPtr);
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
	// AI
	for (auto&& aiPtr : aiPtrs) { aiPtr->think(tools.ui); }
	b2Vec2 mousePos = playerPtr->mouse.b2Ptr->GetPosition();
	// Update cam
	cam.setPosition(glm::vec2(playerPtr->b2Ptr->GetPosition().x, playerPtr->b2Ptr->GetPosition().y));
	cam.update();
	auto batrix = cam.getBaseMatrix();
	auto matrix = cam.getTransScaledMatrix();
#ifdef __STAGING__
	if (tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames || tools.ui.getKeyInfo(fk::Key::CTRL_R).downFrames) {
		if (tools.ui.getKeyInfo(fk::Key::Z).downFrames == 1) {
			Dummy::Def dd;
			dd.body = tools.textures.get("circle128.png", 1);
			dd.position = fk::Vec2(mousePos.x, mousePos.y);
			auto dummy = new Dummy(*spriteBatchPtr, world, dd);
			aiPtrs.push_back(dummy);
			imagePtrs.push_back(dummy);
		}
		if (tools.ui.getKeyInfo(fk::Key::X).downFrames == 1) {
			Dummy::Def dd;
			dd.body = tools.textures.get("circle128.png", 1);
			dd.position = fk::Vec2(mousePos.x, mousePos.y);
			auto dummy = new DumRunner(*spriteBatchPtr, world, dd);
			aiPtrs.push_back(dummy);
			imagePtrs.push_back(dummy);
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM1).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new ScanMarker(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM2).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new Retracer(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM3).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new Booster(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM4).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new SFCharge(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM5).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new Anchor(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM6).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new Cloak(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM7).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new CPC(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM8).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new DetectorDust(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM9).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new HealField(*spriteBatchPtr, ed));
		}
		if (tools.ui.getKeyInfo(fk::Key::NUM0).downFrames == 1) {
			Equipment::Def ed;
			ed.body = tools.textures.get("circle128.png", 1);
			ed.cursorAccesory = tools.textures.get("circle128.png", 1);
			ed.projectileSpawner.body = tools.textures.get("circle128.png", 1);
			playerPtr->setGadget(new ShieldWall(*spriteBatchPtr, ed));
		}
	}
#endif // __STAGING__
	// Physics
	auto body = world.b2Ptr->GetBodyList();
	while (body) {
		auto position = body->GetPosition();
		static_cast<Body*>(body->GetUserData())->update();
		position = body->GetPosition();
		body = body->GetNext();
	}
	world.update(1.0f / 60.0f, 2, 2);
	body = world.b2Ptr->GetBodyList();
	while (body) {
		auto position = body->GetPosition();
		body = body->GetNext();
	}
	// Update pixel
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Render
	//wireRenderer.render(world, matrix);
	if (!playerPtr->statEffects.blind) {
		spriteRenderer.render(*spriteBatchPtr, matrix);
		spriteRenderer.render(*textBatchPtr, batrix);
	}
}
