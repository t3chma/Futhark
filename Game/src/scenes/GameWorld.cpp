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
	fk::Texture t = tools.textures.get("Square.png");
	std::string s = "Arenas/" + levels[currentLevel] + ".area";
	arenaPtr = new Arena(s, font, t, *spriteBatchPtr, world, pd);
	playerPtr->b2Ptr->SetTransform(arenaPtr->spawns[0], 0);
	player2Ptr->b2Ptr->SetTransform(arenaPtr->spawns[1], 0);

	// Editor
	Mouse::Def md;
	md.body = tools.textures.get("Ring.png");
	editorMousePtr = new Mouse(*spriteBatchPtr, world, md);
	editIconPtr = new Icon(*spriteBatchPtr, tools.textures.get("Build.png"));
	editorIconPtr = new Icon(*spriteBatchPtr, tools.textures.get("Build.png"));
	selectionInfoPtr = new fk::TextSprite(*spriteBatchPtr, font);

	// Prepedi
	arenaPtr->draw();
	for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
}
void GameWorld::close(fk::Tools& tools) {
	mode = 'p';

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

	// Editor
	delete editorMousePtr;
	delete editorIconPtr;
	delete editIconPtr;
	selectionInfoPtr->clearText();
	delete selectionInfoPtr;
	edited = false;
	cp = nullptr;
}
void GameWorld::update(fk::Tools& tools) {
	///glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (
		tools.ui.getJoyInfo(fk::Joy::START, 1).downFrames == 1
		|| tools.ui.getJoyInfo(fk::Joy::START, 2).downFrames == 1
		|| tools.ui.getKeyInfo(fk::Key::CTRL_R).downFrames == 1
		|| tools.ui.getKeyInfo(fk::Key::CTRL_L).downFrames == 1
	) {
		if (mode == 'p') { mode = 'g'; }
		else if (mode != 'p') { mode = 'p'; }
	}
	if (
		tools.ui.getJoyInfo(fk::Joy::BACK, 1).downFrames == 180
		|| tools.ui.getKeyInfo(fk::Key::ESC).downFrames == 60
	) {
		std::exit(0);
	}
	if (mode == 'p') {
		for (auto&& level : levelSelect) {
			for (int i = 0; i < level.getTextLength(); ++i) {
				level[i].canvas.color.a = 255;
			}
		}
		editorIconPtr->sprites.back().setColor(0, 0, 0, 0);
		editIconPtr->sprites.back().setColor(0, 0, 0, 0);
		backgroundPtr->getCanvasRef().position.z = 100;
		cam.setPosition(arenaPtr->cam);
		cam.update();
		auto batrix = cam.getBaseMatrix();
		auto matrix = cam.getTransScaledMatrix();
		spriteRenderer.render(*spriteBatchPtr, matrix);
		spriteRenderer.render(*textBatchPtr, batrix);
		if (
			tools.ui.getJoyInfo(fk::Joy::BACK, 1).downFrames == 1
			|| tools.ui.getJoyInfo(fk::Joy::BACK, 2).downFrames == 1
			|| tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 1
			|| tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 1
		) {
			close(tools);
			open(tools);
		} else if (
			tools.ui.getJoyInfo(fk::Joy::BACK, 1).downFrames == 60
			|| tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 60
			|| tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 60
		) {
			if (mode == 'p') { mode = 'e'; }
			else if (mode == 'e') { mode = 'p'; }
		}
		if (
			tools.ui.getJoyInfo(fk::Joy::R, 1).downFrames == 1
			|| tools.ui.getJoyInfo(fk::Joy::R, 2).downFrames == 1
			|| tools.ui.getKeyInfo(fk::Key::RIGHT).downFrames == 1
		) {
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
		} else if (
			tools.ui.getJoyInfo(fk::Joy::L, 1).downFrames == 1
			|| tools.ui.getJoyInfo(fk::Joy::L, 2).downFrames == 1
			|| tools.ui.getKeyInfo(fk::Key::LEFT).downFrames == 1
		) {
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
	} else if (mode == 'g' || mode == 'e') {
		for (auto&& level : levelSelect) {
			for (int i = 0; i < level.getTextLength(); ++i) {
				level[i].canvas.color.a = 0;
			}
		}

		if (mode == 'e') {
			editorMousePtr->update(tools.ui);
			if (edited) {
				editorIconPtr->sprites.back().setColor(255, 0, 0, 255);
				editIconPtr->sprites.back().setColor(255, 0, 0, 255);
			}
			else {
				editorIconPtr->sprites.back().setColor(255, 255, 255, 255);
				editIconPtr->sprites.back().setColor(255, 255, 255, 255);
				if (tools.ui.getKeyInfo(fk::Key::W).downFrames) { cam.move(fk::Vec2(0, 0.1)); }
				if (tools.ui.getKeyInfo(fk::Key::A).downFrames) { cam.move(fk::Vec2(-0.1, 0)); }
				if (tools.ui.getKeyInfo(fk::Key::S).downFrames) { cam.move(fk::Vec2(0, -0.1)); }
				if (tools.ui.getKeyInfo(fk::Key::D).downFrames) { cam.move(fk::Vec2(0.1, 0)); }
			}
		} else {
			editorIconPtr->sprites.back().setColor(0, 0, 0, 0);
			editIconPtr->sprites.back().setColor(0, 0, 0, 0);
			arenaPtr->update(tools.ui);
			cam.setPosition(arenaPtr->cam);
			for (auto&& actorPtr : actorPtrs) { actorPtr->update(tools.ui); }
		}

		// Draw sprites
		arenaPtr->draw();
		for (auto&& imagePtrs : imagePtrs) { imagePtrs->draw(); }
		editorMousePtr->draw();

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
		if (playerPtr->health == 0 || player2Ptr->health == 0) {
			arenaPtr->freezeCam = true;
		}
		cam.update();
		auto batrix = cam.getBaseMatrix();
		auto matrix = cam.getTransScaledMatrix();
		editorIconPtr->sprites.back().setPosition(fk::Vec2(0.5, -0.5) + cam.getLensTopLeft());
		auto p = fk::Vec2(1.5, -0.5) + cam.getLensTopLeft();
		selectionInfoPtr->setPosition(p);

		// Mouse
		fk::Vec2 mousePos = cam.getWorldCoordinates(tools.ui.getMouseInfo(0).windowPosition);
		glm::ivec2 blockIndex((mousePos.x * 2 + 1.5) - 1, -(mousePos.y * 2 + 0.5) + 1);
		editorMousePtr->b2Ptr->SetTransform(mousePos, 0);

		auto p1p = playerPtr->b2Ptr->GetPosition();
		auto p2p = player2Ptr->b2Ptr->GetPosition();
		fk::Vec2 bd = backgroundPtr->getCanvasRef().dimensions / cam.getZoom() / 2;
		fk::Vec2 bp = backgroundPtr->getPosition();
		if (mode == 'e') {
			std::string s = "";
			if (edited) {
				if (tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 1) {
					if (selectionInfoPtr->getText()[5] != '_') {
						*cp = selectionInfoPtr->getText()[5];
					}
					selectionInfoPtr->setText("");
					fk::IOManager io;
					std::string newBuffer = "";
					for (auto&& line : arenaPtr->buffer) { newBuffer += line; }
					s = "Arenas/" + levels[currentLevel] + ".area";
					io.overwriteStringToFile(s, newBuffer);
					auto cpos = cam.getPosition();
					close(tools);
					open(tools);
					mode = 'e';
					cam.setPosition(cpos);
				} else if (tools.ui.getKeyInfo(fk::Key::BACK_SPC).downFrames == 1) {
					selectionInfoPtr->setText("");
					edited = false;
				} else {
					newBlock = '\n';
					if (tools.ui.getKeyInfo(fk::Key::SHIFT_L).downFrames || tools.ui.getKeyInfo(fk::Key::SHIFT_R).downFrames) {
						if (tools.ui.getKeyInfo(fk::Key::SPACE).downFrames == 1) { newBlock = ' '; }
						else if (tools.ui.getKeyInfo(fk::Key::ACCENT).downFrames == 1) { newBlock = '~'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM1).downFrames == 1) { newBlock = '!'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM2).downFrames == 1) { newBlock = '@'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM3).downFrames == 1) { newBlock = '#'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM4).downFrames == 1) { newBlock = '$'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM5).downFrames == 1) { newBlock = '%'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM6).downFrames == 1) { newBlock = '^'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM7).downFrames == 1) { newBlock = '&'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM8).downFrames == 1) { newBlock = '*'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM9).downFrames == 1) { newBlock = '('; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM0).downFrames == 1) { newBlock = ')'; }
						else if (tools.ui.getKeyInfo(fk::Key::EQUAL).downFrames == 1) { newBlock = '+'; }
						else if (tools.ui.getKeyInfo(fk::Key::Q).downFrames == 1) { newBlock = 'Q'; }
						else if (tools.ui.getKeyInfo(fk::Key::W).downFrames == 1) { newBlock = 'W'; }
						else if (tools.ui.getKeyInfo(fk::Key::E).downFrames == 1) { newBlock = 'E'; }
						else if (tools.ui.getKeyInfo(fk::Key::R).downFrames == 1) { newBlock = 'R'; }
						else if (tools.ui.getKeyInfo(fk::Key::T).downFrames == 1) { newBlock = 'T'; }
						else if (tools.ui.getKeyInfo(fk::Key::Y).downFrames == 1) { newBlock = 'Y'; }
						else if (tools.ui.getKeyInfo(fk::Key::U).downFrames == 1) { newBlock = 'U'; }
						else if (tools.ui.getKeyInfo(fk::Key::I).downFrames == 1) { newBlock = 'I'; }
						else if (tools.ui.getKeyInfo(fk::Key::O).downFrames == 1) { newBlock = 'O'; }
						else if (tools.ui.getKeyInfo(fk::Key::P).downFrames == 1) { newBlock = 'P'; }
						else if (tools.ui.getKeyInfo(fk::Key::BRACKET_L).downFrames == 1) { newBlock = '{'; }
						else if (tools.ui.getKeyInfo(fk::Key::BRACKET_R).downFrames == 1) { newBlock = '}'; }
						else if (tools.ui.getKeyInfo(fk::Key::SLASH_B).downFrames == 1) { newBlock = '|'; }
						else if (tools.ui.getKeyInfo(fk::Key::A).downFrames == 1) { newBlock = 'A'; }
						else if (tools.ui.getKeyInfo(fk::Key::S).downFrames == 1) { newBlock = 'S'; }
						else if (tools.ui.getKeyInfo(fk::Key::D).downFrames == 1) { newBlock = 'D'; }
						else if (tools.ui.getKeyInfo(fk::Key::F).downFrames == 1) { newBlock = 'F'; }
						else if (tools.ui.getKeyInfo(fk::Key::G).downFrames == 1) { newBlock = 'G'; }
						else if (tools.ui.getKeyInfo(fk::Key::H).downFrames == 1) { newBlock = 'H'; }
						else if (tools.ui.getKeyInfo(fk::Key::J).downFrames == 1) { newBlock = 'J'; }
						else if (tools.ui.getKeyInfo(fk::Key::K).downFrames == 1) { newBlock = 'K'; }
						else if (tools.ui.getKeyInfo(fk::Key::L).downFrames == 1) { newBlock = 'L'; }
						else if (tools.ui.getKeyInfo(fk::Key::SEMICOLON).downFrames == 1) { newBlock = ':'; }
						else if (tools.ui.getKeyInfo(fk::Key::QUOTE).downFrames == 1) { newBlock = '"'; }
						else if (tools.ui.getKeyInfo(fk::Key::Z).downFrames == 1) { newBlock = 'Z'; }
						else if (tools.ui.getKeyInfo(fk::Key::X).downFrames == 1) { newBlock = 'X'; }
						else if (tools.ui.getKeyInfo(fk::Key::C).downFrames == 1) { newBlock = 'C'; }
						else if (tools.ui.getKeyInfo(fk::Key::V).downFrames == 1) { newBlock = 'V'; }
						else if (tools.ui.getKeyInfo(fk::Key::B).downFrames == 1) { newBlock = 'B'; }
						else if (tools.ui.getKeyInfo(fk::Key::N).downFrames == 1) { newBlock = 'N'; }
						else if (tools.ui.getKeyInfo(fk::Key::M).downFrames == 1) { newBlock = 'M'; }
						else if (tools.ui.getKeyInfo(fk::Key::COMMA).downFrames == 1) { newBlock = '<'; }
						else if (tools.ui.getKeyInfo(fk::Key::PERIOD).downFrames == 1) { newBlock = '>'; }
						else if (tools.ui.getKeyInfo(fk::Key::SLASH_F).downFrames == 1) { newBlock = '?'; }
					} else {
						if (tools.ui.getKeyInfo(fk::Key::SPACE).downFrames == 1) { newBlock = ' '; }
						else if (tools.ui.getKeyInfo(fk::Key::ACCENT).downFrames == 1) { newBlock = '`'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM1).downFrames == 1) { newBlock = '1'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM2).downFrames == 1) { newBlock = '2'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM3).downFrames == 1) { newBlock = '3'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM4).downFrames == 1) { newBlock = '4'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM5).downFrames == 1) { newBlock = '5'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM6).downFrames == 1) { newBlock = '6'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM7).downFrames == 1) { newBlock = '7'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM8).downFrames == 1) { newBlock = '8'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM9).downFrames == 1) { newBlock = '9'; }
						else if (tools.ui.getKeyInfo(fk::Key::NUM0).downFrames == 1) { newBlock = '0'; }
						else if (tools.ui.getKeyInfo(fk::Key::MINUS).downFrames == 1) { newBlock = '-'; }
						else if (tools.ui.getKeyInfo(fk::Key::EQUAL).downFrames == 1) { newBlock = '='; }
						else if (tools.ui.getKeyInfo(fk::Key::Q).downFrames == 1) { newBlock = 'q'; }
						else if (tools.ui.getKeyInfo(fk::Key::W).downFrames == 1) { newBlock = 'w'; }
						else if (tools.ui.getKeyInfo(fk::Key::E).downFrames == 1) { newBlock = 'e'; }
						else if (tools.ui.getKeyInfo(fk::Key::R).downFrames == 1) { newBlock = 'r'; }
						else if (tools.ui.getKeyInfo(fk::Key::T).downFrames == 1) { newBlock = 't'; }
						else if (tools.ui.getKeyInfo(fk::Key::Y).downFrames == 1) { newBlock = 'y'; }
						else if (tools.ui.getKeyInfo(fk::Key::U).downFrames == 1) { newBlock = 'u'; }
						else if (tools.ui.getKeyInfo(fk::Key::I).downFrames == 1) { newBlock = 'i'; }
						else if (tools.ui.getKeyInfo(fk::Key::O).downFrames == 1) { newBlock = 'o'; }
						else if (tools.ui.getKeyInfo(fk::Key::P).downFrames == 1) { newBlock = 'p'; }
						else if (tools.ui.getKeyInfo(fk::Key::BRACKET_L).downFrames == 1) { newBlock = '['; }
						else if (tools.ui.getKeyInfo(fk::Key::BRACKET_R).downFrames == 1) { newBlock = ']'; }
						else if (tools.ui.getKeyInfo(fk::Key::SLASH_B).downFrames == 1) { newBlock = '\\'; }
						else if (tools.ui.getKeyInfo(fk::Key::A).downFrames == 1) { newBlock = 'a'; }
						else if (tools.ui.getKeyInfo(fk::Key::S).downFrames == 1) { newBlock = 's'; }
						else if (tools.ui.getKeyInfo(fk::Key::D).downFrames == 1) { newBlock = 'd'; }
						else if (tools.ui.getKeyInfo(fk::Key::F).downFrames == 1) { newBlock = 'f'; }
						else if (tools.ui.getKeyInfo(fk::Key::G).downFrames == 1) { newBlock = 'g'; }
						else if (tools.ui.getKeyInfo(fk::Key::H).downFrames == 1) { newBlock = 'h'; }
						else if (tools.ui.getKeyInfo(fk::Key::J).downFrames == 1) { newBlock = 'j'; }
						else if (tools.ui.getKeyInfo(fk::Key::K).downFrames == 1) { newBlock = 'k'; }
						else if (tools.ui.getKeyInfo(fk::Key::L).downFrames == 1) { newBlock = 'l'; }
						else if (tools.ui.getKeyInfo(fk::Key::SEMICOLON).downFrames == 1) { newBlock = ';'; }
						else if (tools.ui.getKeyInfo(fk::Key::QUOTE).downFrames == 1) { newBlock = '\''; }
						else if (tools.ui.getKeyInfo(fk::Key::Z).downFrames == 1) { newBlock = 'z'; }
						else if (tools.ui.getKeyInfo(fk::Key::X).downFrames == 1) { newBlock = 'x'; }
						else if (tools.ui.getKeyInfo(fk::Key::C).downFrames == 1) { newBlock = 'c'; }
						else if (tools.ui.getKeyInfo(fk::Key::V).downFrames == 1) { newBlock = 'v'; }
						else if (tools.ui.getKeyInfo(fk::Key::B).downFrames == 1) { newBlock = 'b'; }
						else if (tools.ui.getKeyInfo(fk::Key::N).downFrames == 1) { newBlock = 'n'; }
						else if (tools.ui.getKeyInfo(fk::Key::M).downFrames == 1) { newBlock = 'm'; }
						else if (tools.ui.getKeyInfo(fk::Key::COMMA).downFrames == 1) { newBlock = ','; }
						else if (tools.ui.getKeyInfo(fk::Key::PERIOD).downFrames == 1) { newBlock = '.'; }
						else if (tools.ui.getKeyInfo(fk::Key::SLASH_F).downFrames == 1) { newBlock = '/'; }
					}
					if (newBlock != '\n') {
						s += selectionInfoPtr->getText();
						s[5] = newBlock;
						selectionInfoPtr->setText(s);
					}
				}
			} else {
				if (tools.ui.getKeyInfo(fk::Key::MOUSE_LEFT).downFrames == 1) {
					cp = arenaPtr->getCharAt(blockIndex);
					fk::Vec2 pos = editorMousePtr->sprites.back().getPosition();
					auto ipos = glm::ivec2(pos.x * 2 + 0.5, pos.y * 2 - 0.5);
					pos.x = ipos.x;
					pos.y = ipos.y;
					pos /= 2;
					if (cp) {
						s += *cp;
						editIconPtr->sprites.back().setPosition(pos);
					} else {
						s += " ";
						editIconPtr->sprites.back().setPosition(0, 0);
					}
					selectionInfoPtr->setText(s);
				}
				if (selectionInfoPtr->getTextLength()) {
					if (tools.ui.getKeyInfo(fk::Key::ENTER).downFrames == 1) {
						s += selectionInfoPtr->getText();
						s += " to _";
						selectionInfoPtr->setText(s);
						edited = true;
					}
				}
			}
		} else {
			if (p1p.x < bp.x - bd.x || p1p.x > bp.x + bd.x || p1p.y < bp.y - bd.y || p1p.y > bp.y + bd.y) {
				playerPtr->health = 0;
			}
			if (p2p.x < bp.x - bd.x || p2p.x > bp.x + bd.x || p2p.y < bp.y - bd.y || p2p.y > bp.y + bd.y) {
				player2Ptr->health = 0;
			}
		}
		world.setGravity(
			arenaPtr->gravity.x + playerPtr->gravMod.x + player2Ptr->gravMod.x,
			arenaPtr->gravity.y + playerPtr->gravMod.y + player2Ptr->gravMod.y
		);
		world.update(1.0f / 60.0f, 2, 4);

		spriteRenderer.render(*spriteBatchPtr, matrix);
		spriteRenderer.render(*textBatchPtr, batrix);
		//wireRenderer.render(world, matrix);
	}
}

Icon::Icon(fk::SpriteBatch& sb, fk::Texture t) : Image(sb) {
	sprites.emplace_back(sb, t);
}
