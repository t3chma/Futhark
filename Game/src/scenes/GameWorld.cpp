#include "GameWorld.h"
#include "base\Utility.h"
#include <iostream>
#include "in/IOManager.h"
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

void GameWorld::create(fk::Tools& tools) {
	///tools.ui.setShowCursor(false);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	// Set up sprite batch
	spriteBatchPtr = new fk::SpriteBatch(true);
	backgroundPtr = new fk::Sprite(*spriteBatchPtr, tools.textures.get("Square.png"));
	backgroundPtr->getCanvasRef().position.z = -100;
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

	// Levels
	std::string path("Arenas/");
	std::string ext(".area");
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(path)) {
		auto name = p.path().stem().string();
		if (p.path().extension() == ext) { levels.push_back(name); }
	}

	// Text
	font = tools.fonts.get("consola.ttf");

}
void GameWorld::destroy(fk::Tools& tools) {

}
void GameWorld::open(fk::Tools& tools) {
	// Text
	levelSelect.push_back(font.generateCharSprites(
		levels[currentLevel], *spriteBatchPtr, glm::vec2(0.5), fk::Justification::MIDDLE
	));
	levelSelect.back().setDepth(101);
	auto nextLevel = currentLevel;
	if (levels.size() > 1) {
		if (nextLevel == levels.size() - 1) { nextLevel = 0; }
		else { ++nextLevel; }
		levelSelect.push_back(font.generateCharSprites(
			levels[nextLevel], *spriteBatchPtr, glm::vec2(0.5), fk::Justification::MIDDLE
		));
		levelSelect.back().setDepth(99);
	}
	nextLevel = currentLevel;
	if (levels.size() > 2) {
		if (nextLevel == 0) { nextLevel = levels.size() - 1; }
		else { --nextLevel; }
		levelSelect.push_front(font.generateCharSprites(
			levels[nextLevel], *spriteBatchPtr, glm::vec2(0.5), fk::Justification::MIDDLE
		));
		levelSelect.front().setDepth(99);
	}

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
	auto t = tools.textures.get("Square.png");
	auto s = "Arenas/" + levels[currentLevel] + ".area";
	arenaPtr = new Arena(s, font, t, *spriteBatchPtr, world, pd);
	playerPtr->b2Ptr->SetTransform(arenaPtr->spawns[0], 0);
	player2Ptr->b2Ptr->SetTransform(arenaPtr->spawns[1], 0);

	// Prep
	arenaPtr->draw();
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
}
void GameWorld::close(fk::Tools& tools) {
	paused = true;

	// Text
	for (auto&& level : levelSelect) { level.clearText(); }
	levelSelect.clear();

	// Player
	delete playerPtr;
	delete player2Ptr;
	actorPtrs.clear();
	imagePtrs.clear();

	// Arena
	delete arenaPtr;
}
void GameWorld::update(fk::Tools& tools) {
	///glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (tools.ui.getJoyInfo(fk::Joy::START, 1).downFrames == 1) { paused = !paused; }
	if (paused) {
		for (auto&& level : levelSelect) {
			for (int i = 0; i < level.getTextLength(); ++i) {
				level[i].canvas.color.a = 255;
			}
		}
		backgroundPtr->getCanvasRef().position.z = 100;
		cam.setPosition(arenaPtr->cam);
		cam.update();
		auto batrix = cam.getBaseMatrix();
		auto matrix = cam.getTransScaledMatrix();
		spriteRenderer.render(*spriteBatchPtr, matrix);
		spriteRenderer.render(*textBatchPtr, batrix);
		if (tools.ui.getJoyInfo(fk::Joy::BACK, 1).downFrames) {
			++exitCounter;
			if (tools.ui.getJoyInfo(fk::Joy::BACK, 1).downFrames == 1) {
				close(tools);
				open(tools);
			}
		}
		else { exitCounter = 0; }
		if (exitCounter > 60) { std::exit(0); };
		if (tools.ui.getJoyInfo(fk::Joy::R, 1).downFrames == 1) {
			if (levelSelect.size() > 2) {
				if (currentLevel == levels.size() - 1) { currentLevel = 0; }
				else { ++currentLevel; }
				close(tools);
				open(tools);
			} else if (levelSelect.size() > 1) {
				if (currentLevel == 0) { currentLevel = 1; }
				else { currentLevel = 0; }
				close(tools);
				open(tools);
			}
		} else if (tools.ui.getJoyInfo(fk::Joy::L, 1).downFrames == 1) {
			if (levelSelect.size() > 2) {
				if (currentLevel == 0) { currentLevel = levels.size() - 1; }
				else { --currentLevel; }
				close(tools);
				open(tools);
			} else if (levelSelect.size() > 1) {
				if (currentLevel == 0) { currentLevel = 1; }
				else { currentLevel = 0; }
				close(tools);
				open(tools);
			}
		}
		int i = -1;
		for (auto&& level : levelSelect) {
			switch (i++) {
			default: break;
			case -1:
				level.setPosition(arenaPtr->cam + glm::vec2(0,1));
				break;
			case 0:
				level.setPosition(arenaPtr->cam);
				break;
			case 1:
				level.setPosition(arenaPtr->cam + glm::vec2(0, -1));
				break;
			}
		}
	} else {
		for (auto&& level : levelSelect) {
			for (int i = 0; i < level.getTextLength(); ++i) {
				level[i].canvas.color.a = 0;
			}
		}
		//AI
		arenaPtr->update(tools.ui);
		for (auto&& actorPtr : actorPtrs) { actorPtr->update(tools.ui); }

		// Draw sprites
		arenaPtr->draw();
		for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }

		// Mouse
		glm::vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);

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
		backgroundPtr->getCanvasRef().position.z = -100;
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
			playerPtr->health = 0;
		}
		if (p2p.x < bp.x - bd.x || p2p.x > bp.x + bd.x || p2p.y < bp.y - bd.y || p2p.y > bp.y + bd.y) {
			player2Ptr->health = 0;
		}
		world.setGravity(
			arenaPtr->gravity.x + playerPtr->gravMod.x + player2Ptr->gravMod.x,
			arenaPtr->gravity.y + playerPtr->gravMod.y + player2Ptr->gravMod.y
		);
		world.update(1.0f / 60.0f, 3, 2);
	}
}
