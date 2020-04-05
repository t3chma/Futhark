#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>
#include "in/IOManager.h"

void GameWorld::create(fk::Tools& tools) {
	///tools.ui.setShowCursor(false);
	fk::Texture t = tools.textures.get("Square.png");
	
	// Set up sprite batch
	spriteBatchPtr = new fk::SpriteBatch(true);
	backgroundPtr = new fk::Sprite(*spriteBatchPtr, t);
	backgroundPtr->getCanvasRef().position.z = -10;
	backgroundPtr->setColor(128, 128, 128, 150);
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
	backgroundPtr->setDimensions(tools.windowPtr->getDimensions());
	cam.setZoom(80);

	// Text
	font = tools.fonts.get("calibri.ttf");
	//std::string text = "test";
	//font.generateCharSprites(text, *spriteBatchPtr, glm::vec2(0.5));

	// Player
	Player::Def pd;
	pd.body = tools.textures.get("Circle.png");
	pd.shield = tools.textures.get("Ring.png");
	pd.md.body = tools.textures.get("Circle.png");
	pd.hudFont = &font;
	playerPtr = new Player(*spriteBatchPtr, *textBatchPtr, world, pd);
	player2Ptr = new Player(*spriteBatchPtr, *textBatchPtr, world, pd);
	player2Ptr->setTeam(2);
	actorPtrs.push_back(playerPtr);
	imagePtrs.push_back(playerPtr);
	actorPtrs.push_back(player2Ptr);
	imagePtrs.push_back(player2Ptr);

	// Arena
	std::string s = "Arenas/test.area";
	arenaPtr = new Arena(s, font, t, *spriteBatchPtr, world, pd);
	playerPtr->b2Ptr->SetTransform(arenaPtr->spawns[0], 0);
	player2Ptr->b2Ptr->SetTransform(arenaPtr->spawns[1], 0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}
void GameWorld::destroy(fk::Tools& tools) {

}
void GameWorld::open(fk::Tools& tools) {

}
void GameWorld::close(fk::Tools& tools) {

}
void GameWorld::update(fk::Tools& tools) {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//AI
	arenaPtr->update(tools.ui);
	for (auto&& actorPtr : actorPtrs) { actorPtr->update(tools.ui); }
	
	// Draw sprites
	arenaPtr->draw();
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }

	// Mouse
	glm::vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
	// Debug
	if (tools.ui.getKeyInfo(fk::Key::SPACE).downFrames) {
		delete playerPtr;
		delete player2Ptr;
		delete arenaPtr;
		actorPtrs.clear();
		imagePtrs.clear();
		// Player
		Player::Def pd;
		pd.body = tools.textures.get("Circle.png");
		pd.shield = tools.textures.get("Ring.png");
		pd.md.body = tools.textures.get("Circle.png");
		pd.hudFont = &font;
		playerPtr = new Player(*spriteBatchPtr, *textBatchPtr, world, pd);
		player2Ptr = new Player(*spriteBatchPtr, *textBatchPtr, world, pd);
		player2Ptr->setTeam(2);
		actorPtrs.push_back(playerPtr);
		imagePtrs.push_back(playerPtr);
		actorPtrs.push_back(player2Ptr);
		imagePtrs.push_back(player2Ptr);
		// Arena
		std::string s = "Arenas/test.area";
		fk::Texture t = tools.textures.get("Square.png");
		arenaPtr = new Arena(s, font, t, *spriteBatchPtr, world, pd);
		playerPtr->b2Ptr->SetTransform(arenaPtr->spawns[0], 0);
		player2Ptr->b2Ptr->SetTransform(arenaPtr->spawns[1], 0);
	}

	// Update cam and render.
	auto p1 = glm::vec2(playerPtr->b2Ptr->GetPosition().x, playerPtr->b2Ptr->GetPosition().y);
	auto p2 = glm::vec2(player2Ptr->b2Ptr->GetPosition().x, player2Ptr->b2Ptr->GetPosition().y);
	auto v = p2 - p1; v.x /= 2; v.y /= 2;
	p1 += v;
	if (player2Ptr->toggleCam) { playerPtr->toggleCam = true; }
	if (playerPtr->toggleCam) {
		arenaPtr->freezeCam = !arenaPtr->freezeCam;
		playerPtr->toggleCam = false;
		player2Ptr->toggleCam = false;
	}
	backgroundPtr->setPosition(arenaPtr->cam);
	if (playerPtr->health == 0 || player2Ptr->health == 0) { arenaPtr->freezeCam = true; }
	cam.setPosition(arenaPtr->cam);
	cam.update();
	auto batrix = cam.getBaseMatrix();
	auto matrix = cam.getTransScaledMatrix();
	spriteRenderer.render(*spriteBatchPtr, matrix);
	spriteRenderer.render(*textBatchPtr, batrix);
	//wireRenderer.render(world, matrix);
	
	// Physics
	auto p1p = playerPtr->b2Ptr->GetPosition();
	auto p2p = player2Ptr->b2Ptr->GetPosition();
	auto bd = backgroundPtr->getCanvasRef().dimensions / cam.getZoom() / 2;
	auto bp = backgroundPtr->getPosition();
	if (p1p.x < bp.x - bd.x || p1p.x > bp.x + bd.x || p1p.y < bp.y - bd.y || p1p.y > bp.y + bd.y) {
		playerPtr->health = 0; }
	if (p2p.x < bp.x - bd.x || p2p.x > bp.x + bd.x || p2p.y < bp.y - bd.y || p2p.y > bp.y + bd.y) {
		player2Ptr->health = 0; }
	world.setGravity(
		arenaPtr->gravity.x + playerPtr->gravMod.x + player2Ptr->gravMod.x,
		arenaPtr->gravity.y + playerPtr->gravMod.y + player2Ptr->gravMod.y
	);
	world.update(1.0f / 60.0f, 3, 2);
}
