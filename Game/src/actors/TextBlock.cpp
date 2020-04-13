#include "TextBlock.h"
#include "up/World.h"
#include "Player.h"

TextBlock::TextBlock(char c, fk::TTFont& f, fk::SpriteBatch& sb, fk::World& w, Body::Def& bd, fk::Texture& t)
	: Image(sb), Body(w, bd) {
	health = 10;
	type = 0;

	// Physics
	b2Ptr->SetFixedRotation(true);
	switch (c) {
	case 'P':
	case 'Q':
	case 'q':
	case 'p': addCircleLimb(2);
	case '.':
	case 's':
	case 'z':
		addRectangleLimb(0.24, 0.24);
		break;
	default:
		addRectangleLimb(0.25, 0.25);
		break;
	case 'x':
	case '~':
	case 'O':
	case '`':
	case 'e':
	case 'm':
	case 'r':
	case 'h':
	case 'l':
	case 't':
		addCircleLimb(0.24);
		break;
	case 'o':
		addCircleLimb(0.12);
		break;
	}
	limbs.back().b2Ptr->SetDensity(20);
	limbs.back().b2Ptr->SetRestitution(0.8);

	switch (c) {
	default: break;
	case '(':
	case ')':
	case '}':
	case '{':
	case ']':
	case '[':
	case '<':
	case '>':
	case '^':
	case 'v':
	case '`':
	case 'm':
	case 'r':
	case 'h':
	case 'l':
	case 't':
	case 'e':
	case 'P':
	case 'Q':
	case 'p':
	case 'q':
	case 'w': limbs.back().b2Ptr->SetSensor(true); b2Ptr->SetType(b2_staticBody);
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	case '-':
	case '=':
	case ':':
	case '#':
	case '*':
	case '+': b2Ptr->SetType(b2_staticBody); break;
	case 'c':
	case 'O':
	case 'o':
	case 'z':
	case 's':
		resistance = 50;
		limbs.back().b2Ptr->SetDensity(50);
		limbs.back().b2Ptr->SetRestitution(0); 
		break;
	case '~': limbs.back().b2Ptr->SetDensity(1); break;
	case 'x': resistance = 0; break;
	}
	b2Ptr->SetLinearDamping(resistance);

	// Text
	sprites.emplace_back(sb, t);
	sprites.back().setDimensions(0.5, 0.5);
	sprites.back().setPosition(bd.position);
	std::string s = "";
	s += c;
	float size = 0.4;
	float size2 = 0.5;
	switch (c) {
	default:
		texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size2), fk::Justification::MIDDLE));
		texts.back().setPosition(bd.position);
		texts.back()[0].canvas.rotationAxis = bd.position;
		break;
	case 'P':
	case 'Q':
	case 'p':
	case 'q':
		for (size_t i = 0; i < 16; i++) {
			texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
			texts.back().setPosition(bd.position);
			texts.back()[0].canvas.rotationAxis = bd.position;
			texts.back()[0].canvas.rotationAngle = fk::TAU / 16.0 * i;
			texts.back()[0].setDimensions(2, 2);
		}
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
	case 'w':
	case ']':
	case '[':
	case '*':
	case '=':
	case '#':
	case '-':
	case '/':
	case '|':
	case 'r':
	case 'm':
	case '`':
	case 'l':
	case 'h':
	case 't':
	case 'e':
		for (size_t i = 0; i < 4; i++) {
			texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
			texts.back().setPosition(bd.position);
			texts.back()[0].canvas.rotationAxis = bd.position;
			texts.back()[0].canvas.rotationAngle = fk::TAU / 4.0 * i;
		}
		break;
	case '+':
	case 'x':
	case 'c':
	case '.':
		texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
		texts.back().setPosition(bd.position);
		texts.back()[0].canvas.rotationAxis = bd.position;
		texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
		texts.back().setPosition(bd.position);
		texts.back()[0].canvas.rotationAxis = bd.position;
		texts.back()[0].canvas.rotationAngle = fk::TAU / 2.0;
		break;
	case 'z':
	case 's':
	case ':':
		texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
		texts.back().setPosition(bd.position);
		texts.back()[0].canvas.rotationAxis = bd.position;
		texts.push_back(f.generateCharSprites(s, sb, glm::vec2(size), fk::Justification::MIDDLE));
		texts.back().setPosition(bd.position);
		texts.back()[0].canvas.rotationAxis = bd.position;
		texts.back()[0].canvas.rotationAngle = fk::TAU / 4.0;
		break;
	}

	for (auto&& text : texts) {
		switch (text.getText()[0]) {
		default:
			text[0].setColor(255, 255, 255, 200);
			sprites.back().setColor(0, 0, 0, 200);
			break;
		case '(':
		case ')':
			text[0].setColor(0, 0, 0, 255);
			sprites.back().setColor(0, 0, 0, 0);
			break;
			// Objects
		case 'o':
			text[0].setDimensions(0.4, 0.4);
		case 'O':
			text[0].setColor(0, 0, 0, 200);
			sprites.back().setColor(0, 0, 0, 0);
			break;
		case 'P':
		case 'p':
			text[0].setColor(0, 0, 0, 10);
			text.setDepth(-10);
			sprites.back().setColor(0, 0, 0, 200);
			break;
		case 'Q':
		case 'q':
			text[0].setColor(255, 255, 255, 10);
			text.setDepth(-10);
			sprites.back().setColor(0, 0, 0, 200);
			break;
		case '`':
		case 'm':
		case 'r':
		case 'h':
		case 'l':
		case 'e':
		case 't':
			text[0].setDimensions(0.4, 0.4);
		case '~':
		case 'c':
			text[0].setColor(0, 255, 0, 200);
			sprites.back().setColor(0, 0, 0, 0);
			break;
		case 'x':
			text[0].setColor(255, 0, 0, 200);
			sprites.back().setColor(0, 0, 0, 0);
			break;
			// Squares
		case '+':
			text[0].setColor(0, 0, 0, 0);
			sprites.back().setColor(255, 0, 0, 255);
			break;
			// Plates
		case '<':
		case '>':
		case '^':
		case 'v':
			text[0].setColor(128, 128, 128, 200);
			sprites.back().setColor(0, 0, 0, 100
);
			sprites.back().getCanvasRef().position.z = -1;
			break;
		case 'w':
			text[0].setColor(0, 0, 255, 100);
			sprites.back().setColor(0, 0, 0, 0);
			break;
		case '}':
			text[0].setColor(0, 0, 0, 0);
			sprites.back().setColor(205, 205, 205, 128);
			break;
		case '{':
			text[0].setColor(0, 0, 0, 0);
			sprites.back().setColor(50, 50, 50, 128);
			break;
		case ']':
			text[0].setColor(0, 255, 0, 0);
			sprites.back().setColor(0, 255, 0, 32);
			break;
		case '[':
			text[0].setColor(255, 0, 0, 0);
			sprites.back().setColor(255, 0, 0, 32);
			break;
			// Blocks
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		case '*':
			text[0].setColor(255, 0, 0, 200);
			sprites.back().setColor(0, 0, 0, 200);
			break;
		case 'z':
		case 's':
		case '#':
		case ':':
		case '.':
			sprites.back().setColor(0, 0, 0, 200);
			text[0].setColor(128, 128, 128, 200);
			break;
		case '/':
		case '|':
			text[0].setColor(0, 0, 255, 255);
			sprites.back().setColor(0, 0, 0, 200);
			break;
		case '-':
			text[0].setColor(0, 0, 0, 200);
			sprites.back().setColor(255, 0, 0, 200);
			break;
		case '=':
			sprites.back().setColor(0, 0, 0, 200);
			text[0].setColor(0, 0, 0, 0);
			break;
		}
	}
}

TextBlock::~TextBlock() { for (auto&& text : texts) { text.clearText(); } }

void TextBlock::setChar(char c) {
	std::string s = "";
	s += c;
	for (auto&& text : texts) { text.setText(s); }
}

void TextBlock::draw() {
	auto position = b2Ptr->GetPosition();
	sprites.back().setPosition(position.x, position.y);
	for (auto&& text : texts) {
		text[0].setPosition(position.x, position.y);
		text[0].setRotationAxis(position.x, position.y);
		int rot = 1;
		switch (text.getText()[0]) {
		default: break;
		case 'P':
		case 'p': rot = -64; break;
		case 'Q':
		case 'q': rot = 64; break;
		case 'O':
		case 'o':
		case 'c':
		case '~':
		case 'x': rot = 8; break;
		}
		if (rot != 1) {
			text[0].canvas.rotationAngle += fk::TAU / rot;
		}
	}
}

void TextBlock::update(fk::UserInput& ui) {
	auto position = b2Ptr->GetPosition();
	position *= 2;
	float x = round(position.x);
	float y = round(position.y);
	position.x -= x;
	position.y -= y;
	float a = 0.05, i = 32;
	float b = 0.03, j = 16;
	float c = 0.0001, k = 4;
	std::string s;
	char upgrade = texts.front().getText()[0];
	switch (upgrade) {
	default: break;
	case 'o':
		if (reactors.size() > 1) {
			for (auto&& r : reactors) { r->health = 0; }
			setChar('O');
			for (auto&& text : texts) { text[0].setColor(0, 0, 0, 255); }
			b2Ptr->DestroyFixture(limbs.back().b2Ptr);
			limbs.pop_back();
			addCircleLimb(0.2);
			limbs.back().b2Ptr->SetDensity(20);
			reactors.clear();
		}
		i = 0; j = 0; k = 0;
		break;
	case 'O':
		if (reactors.size() > 1) {
			for (auto&& r : reactors) { r->health = 0; }
			if (rangen.getInt(0, 1)) {
				setChar('~');
			} else {
				switch (rangen.getInt(0, 5)) {
				default: break;
				case 0: setChar('`'); break;
				case 1: setChar('m'); break;
				case 2: setChar('r'); break;
				case 3: setChar('h'); break;
				case 4: setChar('e'); break;
				case 5: setChar('l'); break;
				case 6: setChar('t'); break;
				}
				texts.back()[0].setDimensions(0.4, 0.4);
			}
			for (auto&& text : texts) { text[0].setColor(0, 255, 0, 255); }
			sprites.back().setColor(0, 0, 0, 0);
			b2Ptr->DestroyFixture(limbs.back().b2Ptr);
			limbs.pop_back();
			addCircleLimb(0.24);
			limbs.back().b2Ptr->SetDensity(1);
		}
		break;
	case 'c': i = 300; j = 200; break;
	case '`':
	case 'm':
	case 'r':
	case 'h':
	case 'l':
	case 'e':
	case 't':
		for (auto&& r : reactors) {
			Player* pod = static_cast<Player*>(r);
			if (pod->interact == 1) {
				char u = pod->gunPtr->upgrade;
				pod->gunPtr->setUpgrade(upgrade);
				pod->gunPtr->text.setText(texts.front().getText());
				pod->gunPtr->text[0].setColor(0, 255, 0, 255);
				pod->gunPtr->text[0].setDimensions(0.3, 0.3);
				setChar(u);
			}
		}
	case '=':
	case '-':
	case '+':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'x':
	case 's':
	case '.': i = 0; j = 0; k = 0; break;
	case '0':
		if (reactors.size()) {
			setChar('x');
			texts.back().clearText();
			texts.pop_back();
			texts.back().clearText();
			texts.pop_back();
			texts.back().clearText();
			texts.pop_back();
			texts.back()[0].setColor(255, 0, 0, 255);
			sprites.back().setColor(255, 0, 0, 0);
			b2Ptr->SetType(b2_dynamicBody);
			b2Ptr->SetLinearDamping(0);
			resistance = 0;
			b2Ptr->DestroyFixture(limbs.back().b2Ptr);
			limbs.pop_back();
			addCircleLimb(0.24);
			limbs.back().b2Ptr->SetDensity(1);
			limbs.back().b2Ptr->SetRestitution(.7);
		}
		i = 300; j = 200; break;
	case '}':
		for (auto&& r : reactors) {
			if (static_cast<Player*>(r)->shield) { r->b2Ptr->SetLinearDamping(0.1); }
			else { r->b2Ptr->SetLinearDamping(0.1); }
		}
		break;
	case '{':
		for (auto&& r : reactors) {
			if (static_cast<Player*>(r)->shield) { r->b2Ptr->SetLinearDamping(100); }
			else { r->b2Ptr->SetLinearDamping(50); }
		}
		break;
	case ']':
		for (auto&& r : reactors) {
			if (r->timers++ == 0) {
				if (++r->timer > 30) {
					r->timer = 0;
					--r->damage;
					static_cast<Player*>(r)->sprites.front().setColor(0, 255, 0, 255);
				}
			}
		}
		break;
	case '[':
		for (auto&& r : reactors) {
			if (r->timers++ == 0) {
				if (++r->timer > 20) {
					r->timer = 0;
					++r->damage;
					static_cast<Player*>(r)->sprites.front().setColor(255, 0, 0, 255);
				}
			}
		}
		break;
	case 'w':
		if (sprites.back().getCanvasRef().color.r) {
			if (damage > 60) {
				for (auto&& text : texts) { text[0].setColor(0, 0, 255, 100); }
				sprites.back().setColor(0, 0, 0, 0);
				damage = 0;
			}
			damage += 1;
		}
		break;
	case '*':
		if (limbs.back().b2Ptr->IsSensor()) {
			addCircleLimb(2);
			limbs.back().b2Ptr->SetSensor(true);
		}
		for (auto&& r : reactors) {
			fk::Vec2 f = r->b2Ptr->GetPosition() - b2Ptr->GetPosition();
			auto l = f.length();
			l *= l * l;
			switch (r->type) {
			default: break;
			case 0: r->b2Ptr->ApplyForceToCenter(b2Vec2(2000.0 * f.x / l, 2000.0 * f.y / l), true); break;
			case 1: r->b2Ptr->ApplyForceToCenter(b2Vec2(200.0 * f.x / l, 200.0 * f.y / l), true); break;
			case 2: r->b2Ptr->ApplyForceToCenter(b2Vec2(500.0 * f.x / l, 500.0 * f.y / l), true); break;
			}
		}
		if (limbs.size() > 1) {
			if (damage > 10) { health = -1; }
			damage += 5;
		}
		break;
	case 'Q':
	case 'q':
		for (auto&& r : reactors) {
			fk::Vec2 f = r->b2Ptr->GetPosition() - b2Ptr->GetPosition();
			auto l = f.length();
			l *= l * l;
			switch (r->type) {
			default: break;
			case 0: r->b2Ptr->ApplyForceToCenter(b2Vec2(50.0 * f.x / l, 50.0 * f.y / l), true); break;
			case 1: r->b2Ptr->ApplyForceToCenter(b2Vec2(10.0 * f.x / l, 10.0 * f.y / l), true); break;
			case 2: r->b2Ptr->ApplyForceToCenter(b2Vec2(50.0 * f.x / l, 50.0 * f.y / l), true); break;
			}
		}
		break;
	case 'P':
	case 'p':
		for (auto&& r : reactors) {
			fk::Vec2 f = r->b2Ptr->GetPosition() - b2Ptr->GetPosition();
			auto l = f.length();
			l *= l * l;
			switch (r->type) {
			default: break;
			case 0: r->b2Ptr->ApplyForceToCenter(b2Vec2(-50.0 * f.x / l, -50.0 * f.y / l), true); break;
			case 1: r->b2Ptr->ApplyForceToCenter(b2Vec2(-10.0 * f.x / l, -10.0 * f.y / l), true); break;
			case 2: r->b2Ptr->ApplyForceToCenter(b2Vec2(-100.0 * f.x / l, -100.0 * f.y / l), true); break;
			}
		}
		break;
	};
	if (position.x > a) { b2Ptr->ApplyForceToCenter(b2Vec2(-i, 0), true); }
	else if (position.x < -a) { b2Ptr->ApplyForceToCenter(b2Vec2(i, 0), true); }
	if (position.y > a) { b2Ptr->ApplyForceToCenter(b2Vec2(0, -i), true); }
	else if (position.y < -a) { b2Ptr->ApplyForceToCenter(b2Vec2(0, i), true); }
	if (position.x > b) { b2Ptr->ApplyForceToCenter(b2Vec2(-j, 0), true); }
	else if (position.x < -b) { b2Ptr->ApplyForceToCenter(b2Vec2(j, 0), true); }
	if (position.y > b) { b2Ptr->ApplyForceToCenter(b2Vec2(0, -j), true); }
	else if (position.y < -b) { b2Ptr->ApplyForceToCenter(b2Vec2(0, j), true); }
	if (position.x > c) { b2Ptr->ApplyForceToCenter(b2Vec2(-k, 0), true); }
	else if (position.x < -c) { b2Ptr->ApplyForceToCenter(b2Vec2(k, 0), true); }
	if (position.y > c) { b2Ptr->ApplyForceToCenter(b2Vec2(0, -k), true); }
	else if (position.y < -c) { b2Ptr->ApplyForceToCenter(b2Vec2(0, k), true); }
}

void TextBlock::p_beginCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	Limb* lim = static_cast<Limb*>(collisionFixturePtr->GetUserData());
	if (!collisionFixturePtr->IsSensor() || bod->type == 2) {
		if (health > 0 && collisionFixturePtr->GetBody()->GetUserData()) {
			auto f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
			glm::vec2 u = glm::normalize(glm::vec2(f.x, f.y));
			u *= 310.0;
			char c = texts.front().getText()[0];
			// Player related
			if (bod->type > 0) {
				auto v = bod->b2Ptr->GetLinearVelocity();
				// Player
				if (bod->type == 1) { hitPlayer(static_cast<Player*>(bod), u); }
				// Bullet
				else { hitBullet(bod, myFixturePtr); }
				switch (c) {
				default: break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					setChar(c - 1);
					for (auto&& text : texts) { text[0].setColor(255, 0, 0, 128); }
					break;
				case '0':
					reactors.push_back(nullptr);
					break;
				}
			} else if (bod->type == 0) { // Envorinment
				auto block = static_cast<TextBlock*>(bod);
				hitBlock(block, myFixturePtr);
			}
			switch (c) {
			default: break;
			case '{':
			case '}':
			case 'P':
			case 'Q':
			case 'q':
			case 'p':
			case '*':
				if (myFixturePtr->IsSensor()) {
					reactors.push_back(bod);
				}
				break;
			}
		}
	}
}

void TextBlock::hitBlock(TextBlock* block, b2Fixture* myFixturePtr) {
	switch (texts.front().getText()[0]) {
	default: break;
	case '-':
	case 'x':
	case '+':
		if (block->texts.size()) {
			switch (block->texts.front().getText()[0]) {
			default: break;
			case 'c':
			case '~': health = 0;  block->health = 0; break;
			}
		}
		break;
	case 'o':
		if (block->texts.front().getText()[0] == 'o') reactors.push_back(block);
		break;
	case 'O':
		if (block->texts.front().getText()[0] == 'O') reactors.push_back(block);
		break;
	}
}

void TextBlock::hitBullet(Body* bod, b2Fixture* myFixturePtr) {
	switch (texts.front().getText()[0]) {
	default: break;
	case '-':
	case ':':
	case '.':
		if (bod->health > 1) {
			--health;
			if (bod->level == 1) { health = 0; }
			sprites.front().getCanvasRef().color.a -= 20;
		}
		break;
	case '#':
		if (bod->health > 1) {
			bod->ownerPtr->toggleCam = true;
			if (sprites.back().getCanvasRef().color.r) {
				sprites.back().setColor(0, 0, 0, 200);
			} else {
				sprites.back().setColor(255, 255, 255, 200);
			}
		}
		break;
	case '*':
		if (limbs.size() < 2) {
			limbs.back().b2Ptr->SetSensor(true);
			sprites.back().setColor(0, 0, 0, 0);
		}
		break;
	case 'q':
	case 'p':
		if (!myFixturePtr->IsSensor() && bod->health > 1 && bod->level == 1) {
			health = 0;
		}
		break;
	}
}

void TextBlock::hitPlayer(Player* pod, glm::vec2 &u) {
	char upgrade = pod->gunPtr->upgrade;
	switch (texts.front().getText()[0]) {
	default: break;
	case '[':
	case ']':
	case '`':
	case 'm':
	case 'r':
	case 'h':
	case 'l':
	case 'e':
	case 't': if (health) { reactors.push_back(pod); } break;
	case '/': pod->torque += fk::TAU / 2; break;
	case '|': pod->gorque += fk::TAU / 2; break;
	case 'w':
		if (
			sprites.front().getCanvasRef().color.r == 255
			&& sprites.front().getCanvasRef().color.g == 0
			&& sprites.front().getCanvasRef().color.b == 0
			&& (pod->reflect >= pod->reflectime || pod->shield)
		) {
			--pod->health;
		}
		break;
	case '-':
	case '+':
	case 'x': if (pod->reflect >= pod->reflectime || pod->shield) { --pod->health; break; }
	case 'c':
	case 'o':
		pod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 3, u.y * 3), true);
		b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, -u.y), true);
		break;
	case 'O':
		if (abs(u.x) > abs(u.y)) {
			pod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 6, 0), true);
			b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, 0), true);
		}
		else {
			pod->b2Ptr->ApplyForceToCenter(b2Vec2(0, u.y * 6), true);
			b2Ptr->ApplyForceToCenter(b2Vec2(0, -u.y), true);
		}
		break;
	case '<':
		pod->gravMod.x -= 8;
		sprites.back().setColor(255, 255, 255, 100);
		break;
	case '>':
		pod->gravMod.x += 8;
		sprites.back().setColor(255, 255, 255, 100);
		break;
	case '^':
		pod->gravMod.y += 8;
		sprites.back().setColor(255, 255, 255, 100);
		break;
	case 'v':
		pod->gravMod.y -= 8;
		sprites.back().setColor(255, 255, 255, 100);
		break;
	}
}

void TextBlock::p_endCollision(b2Fixture* collisionFixturePtr, b2Fixture* myFixturePtr, b2Contact* contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	if (
		health > 0 && collisionFixturePtr->GetBody()->GetUserData() && (
			!collisionFixturePtr->IsSensor() || bod->type == 2
		)
	) {
		switch (texts.front().getText()[0]) {
		default: break;
		case '{':
		case '}':
			reactors.remove(bod);
			if (static_cast<Player*>(bod)->shield) { bod->b2Ptr->SetLinearDamping(bod->resistance * 2); }
			else { bod->b2Ptr->SetLinearDamping(bod->resistance); };
			break;
		case '*':
		case 'P':
		case 'Q':
		case 'p':
		case 'q':
			if (myFixturePtr->IsSensor() && reactors.size()) { reactors.remove(bod); }
			break;
		case '[':
		case ']':
			reactors.remove(bod);
			break;
		}
		// Player related
		if (bod->type > 0) {
			// Player
			if (bod->type == 1) {
				switch (texts.front().getText()[0]) {
				default: break;
				case 'w':
					for (auto&& text : texts) { text[0].setColor(0, 0, 255, 200); }
					sprites.back().setColor(255, 0, 0, 255);
					break;
				case '<':
					bod->gravMod.x += 8;
					sprites.back().setColor(0, 0, 0, 100);
					break;
				case '>':
					bod->gravMod.x -= 8;
					sprites.back().setColor(0, 0, 0, 100);
					break;
				case '^':
					bod->gravMod.y -= 8;
					sprites.back().setColor(0, 0, 0, 100);
					break;
				case 'v':
					bod->gravMod.y += 8;
					sprites.back().setColor(0, 0, 0, 100);
					break;
				case '`':
				case 'm':
				case 'r':
				case 'h':
				case 'l':
				case 'e':
				case 't': reactors.remove(bod); break;
				}
			}
		} else if (bod->type == 0) {
			TextBlock* block = static_cast<TextBlock*>(bod);
			switch (texts.front().getText()[0]) {
			default: break;
			case 'o':
				if (reactors.size() && block->texts.size() && block->texts.front().getText()[0] == 'o') {
					reactors.remove(block);
				}
				break;
			case 'O':
				if (reactors.size() && block->texts.size() && block->texts.front().getText()[0] == 'O') {
					reactors.remove(block);
				}
				break;
			}
		}
	}
}