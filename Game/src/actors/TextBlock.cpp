#include "TextBlock.h"
#include "up/World.h"


TextBlock::TextBlock(fk::TextSprite& text, fk::TextSprite& text2, fk::SpriteBatch& sb, fk::World& w, Body::Def& bd, fk::Texture& t)
	: Image(sb), text(text), text2(text2), Body(w, bd) {
	health = 10;
	type = 0;
	text.setPosition(bd.position);
	text2.setPosition(bd.position);
	text2[0].canvas.rotationAngle += fk::TAU / 2.0;
	p_sprites.emplace_back(sb, t);
	p_sprites.back().setDimensions(0.5, 0.5);
	p_sprites.back().setColor(0, 0, 0, 200);
	p_sprites.back().setPosition(bd.position);
	b2Ptr->SetFixedRotation(true);
	b2Ptr->SetLinearDamping(5);
	if (text.getText()[0] == 'o') {
		addCircleLimb(0.12);
	} else if (text.getText()[0] == 'O' || text.getText()[0] == '~' || text.getText()[0] == 'x') {
		addCircleLimb(0.24);
	} else {
		addRectangleLimb(0.24, 0.24);
	}
	limbs.back().b2Ptr->SetDensity(20);
	switch (text.getText()[0]) {
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
	case '0': text[0].setColor(255, 255, 0, 128); text2[0].setColor(255, 255, 0, 128); b2Ptr->SetType(b2_staticBody); break;
	case '-':
		b2Ptr->SetType(b2_staticBody);
		text[0].setColor(255, 255, 0, 255);
		text2[0].setColor(255, 255, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 255);
		break;
	case '+':
		b2Ptr->SetType(b2_staticBody);
		text[0].setColor(255, 255, 0, 0);
		text2[0].setColor(255, 255, 0, 0);
		p_sprites.back().setColor(255, 0, 0, 255);
		break;
	case '~':
		limbs.back().b2Ptr->SetDensity(1);
		text[0].setColor(0, 255, 0, 255);
		text2[0].setColor(0, 255, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 0);
		break;
	case 'x':
		b2Ptr->SetLinearDamping(0);
		limbs.back().b2Ptr->SetDensity(1);
		limbs.back().b2Ptr->SetRestitution(.7);
		text[0].setColor(255, 0, 0, 255);
		text2[0].setColor(255, 0, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 0);
		break;
	case 'c':
		b2Ptr->SetLinearDamping(5);
		text[0].setColor(0, 255, 0, 255);
		text2[0].setColor(0, 255, 0, 255);
		p_sprites.back().setColor(128, 128, 128, 0);
		break;
	case 'O':
	case 'o':
		b2Ptr->SetLinearDamping(5);
		text[0].setColor(0, 0, 255, 255);
		text2[0].setColor(0, 0, 255, 255);
		p_sprites.back().setColor(128, 128, 128, 0);
		break;
	case '=':
		b2Ptr->SetType(b2_staticBody);
		p_sprites.back().setColor(0, 0, 0, 200);
		text[0].setColor(128, 128, 128, 0);
		text2[0].setColor(128, 128, 128, 0);
		break;
	case 'z':
		b2Ptr->SetLinearDamping(10);
		p_sprites.back().setColor(0, 0, 0, 200);
		text[0].setColor(128, 128, 128, 128);
		text2[0].setColor(128, 128, 128, 128);
		break;
	case 's':
		b2Ptr->SetLinearDamping(10);
		p_sprites.back().setColor(0, 0, 0, 200);
		text[0].setColor(128, 128, 128, 255);
		text2[0].setColor(128, 128, 128, 255);
		break;
	case '#':
	case ':':
		b2Ptr->SetType(b2_staticBody);
		p_sprites.back().setColor(0, 0, 0, 200);
		text[0].setColor(128, 128, 128, 255);
		text2[0].setColor(128, 128, 128, 255);
		break;
	case 'w':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(255, 0, 0, 128);
		text2[0].setColor(255, 0, 0, 128);
		p_sprites.back().setColor(255, 255, 0, 255);
		break;
	case '}':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(0, 0, 0, 0);
		text2[0].setColor(0, 0, 0, 0);
		p_sprites.back().setColor(0, 255, 255, 128);
		break;
	case '{':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(0, 0, 0, 0);
		text2[0].setColor(0, 0, 0, 0);
		p_sprites.back().setColor(255, 0, 255, 128);
		break;
	case ']':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(0, 0, 0, 0);
		text2[0].setColor(0, 0, 0, 0);
		p_sprites.back().setColor(0, 255, 0, 128);
		break;
	case '[':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(0, 0, 0, 0);
		text2[0].setColor(0, 0, 0, 0);
		p_sprites.back().setColor(255, 255, 0, 128);
		break;
	case '*':
		b2Ptr->SetType(b2_staticBody);
		text[0].setColor(255, 0, 0, 255);
		text2[0].setColor(255, 0, 0, 255);
		p_sprites.back().setColor(255, 255, 0, 200);
		break;
	case '<':
	case '>':
	case '^':
	case 'v':
		b2Ptr->SetType(b2_staticBody);
		limbs.back().b2Ptr->SetSensor(true);
		text[0].setColor(128, 128, 128, 255);
		text2[0].setColor(128, 128, 128, 255);
		text2[0].canvas.rotationAngle -= fk::TAU / 2.0;
		p_sprites.back().setColor(0, 0, 0, 100);
		break;
	}
}

TextBlock::~TextBlock() {
	text.clearText();
	text2.clearText();
}

void TextBlock::setChar(char c) {
	std::string s = "" + c;
	text.setText(s);
	text2.setText(s);
	text2[0].canvas.rotationAngle = fk::TAU / 2.0;
}

void TextBlock::draw() {
	auto position = b2Ptr->GetPosition();
	p_sprites.back().setPosition(position.x, position.y);
	text.setPosition(glm::vec2(position.x, position.y));
	text2.setPosition(glm::vec2(position.x, position.y));
	text[0].setRotationAxis(position.x, position.y);
	text2[0].setRotationAxis(position.x, position.y);
	int rot = 4;
	switch (text.getText()[0]) {
	default: break;
	case 'O':
	case 'o':
	case 'c':
	case '~':
	case 'x': text2[0].canvas.color.a = 0; rot = 8; break;
	case '>':
	case '<':
	case '^':
	case 'v': rot = 1; break;
	}
	if (rot != 1) {
		text[0].canvas.rotationAngle += fk::TAU / rot;
		text2[0].canvas.rotationAngle += fk::TAU / rot;
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
	switch (text.getText()[0]) {
	default: break;
	case '*':
		if (limbs.back().b2Ptr->IsSensor()) {
			addCircleLimb(2);
			limbs.back().b2Ptr->SetSensor(true);
		}
		break;
	case 'o':
		if (reactors.size() > 1) {
			for (auto&& r : reactors) { r->health = 0; }
			std::string s = "O";
			text.setText(s);
			text2.setText(s);
			text2[0].canvas.rotationAngle += fk::TAU / 2.0;
			text[0].setColor(0, 0, 255, 255);
			text2[0].setColor(0, 0, 255, 255);
			p_sprites.back().setColor(128, 128, 128, 0);
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
			std::string s = "c";
			text.setText(s);
			text2.setText(s);
			text2[0].canvas.rotationAngle += fk::TAU / 2.0;
			text[0].setColor(0, 255, 0, 255);
			text2[0].setColor(0, 255, 0, 255);
			p_sprites.back().setColor(128, 128, 128, 0);
			b2Ptr->DestroyFixture(limbs.back().b2Ptr);
			limbs.pop_back();
			addCircleLimb(0.12);
			limbs.back().b2Ptr->SetDensity(20);
		}
		break;
	case 'c':
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
	case '9': i = 300; j = 200; break;
	case '0':
		if (reactors.size()) {
			s = "x";
			text.setText(s);
			text2.setText(s);
			text2[0].canvas.rotationAngle += fk::TAU / 2.0;
			text[0].setColor(255, 0, 0, 255);
			text2[0].setColor(255, 0, 0, 255);
			p_sprites.back().setColor(255, 0, 0, 0);
			b2Ptr->SetType(b2_dynamicBody);
			b2Ptr->SetLinearDamping(0);
			b2Ptr->DestroyFixture(limbs.back().b2Ptr);
			limbs.pop_back();
			addCircleLimb(0.24);
			limbs.back().b2Ptr->SetDensity(1);
			limbs.back().b2Ptr->SetRestitution(.7);
		}
		i = 300; j = 200; break;
	case 'x':
	case 's': i = 0; j = 0; k = 0; break;
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

void TextBlock::p_beginCollision(b2Fixture * collisionFixturePtr, b2Fixture * myFixturePtr, b2Contact * contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	auto f = bod->b2Ptr->GetPosition() - b2Ptr->GetPosition();
	glm::vec2 u = glm::normalize(glm::vec2(f.x, f.y));
	u *= 310.0;
	// Player related
	if (bod->type > 0) {
		auto v = bod->b2Ptr->GetLinearVelocity();
		// Player
		if (bod->type == 1) {
			switch (text.getText()[0]) {
			default: break;
			case '}': bod->b2Ptr->SetLinearVelocity(b2Vec2(v.x * 2, v.y * 2)); break;
			case '/': bod->torque += fk::TAU / 2; break;
			case '|': bod->gorque += fk::TAU / 2; break;
			case 'w':
				if (
					p_sprites.front().getCanvasRef().color.r == 255
					&& p_sprites.front().getCanvasRef().color.g == 0
					&& p_sprites.front().getCanvasRef().color.b == 0
				) {
					--bod->health;
				}
				break;
			case '-':
			case '+':
			case 'x': --bod->health; break;
			case '[': ++bod->damage; break;
			case ']': if (--bod->damage < 0) { bod->damage = 0; } break;
			case 'c':
			case 'o':
				bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 2, u.y * 2), true);
				b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, -u.y), true);
				break;
			case 'O':
				if (abs(u.x) > abs(u.y)) {
					bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 5, 0), true);
					b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, 0), true);
				} else {
					bod->b2Ptr->ApplyForceToCenter(b2Vec2(0, u.y * 5), true);
					b2Ptr->ApplyForceToCenter(b2Vec2(0, -u.y), true);
				}
				break;
			case '<':
				bod->gravMod.x -= 10;
				p_sprites.back().setColor(255, 255, 255, 100);
				break;
			case '>':
				bod->gravMod.x += 10;
				p_sprites.back().setColor(255, 255, 255, 100);
				break;
			case '^':
				bod->gravMod.y += 10;
				p_sprites.back().setColor(255, 255, 255, 100);
				break;
			case 'v':
				bod->gravMod.y -= 10;
				p_sprites.back().setColor(255, 255, 255, 100);
				break;
			}
		// Player made
		} else {
			switch (text.getText()[0]) {
			default: break;
			case '-':
			case ':':
				--health;
				if (bod->health > 120) { health = 0; }
				p_sprites.front().getCanvasRef().color.a -= 20;
				break;
			case 'w':
				limbs.back().b2Ptr->SetSensor(true);
				p_sprites.back().setColor(255, 255, 0, 255);
				break;
			case '#':
				bod->ownerPtr->toggleCam = true;
				if (p_sprites.back().getCanvasRef().color.r) {
					p_sprites.back().setColor(0, 0, 0, 200);
				} else {
					p_sprites.back().setColor(255, 255, 255, 200);
				}
				break;
			case '*':
				if (limbs.size() < 2) {
					limbs.back().b2Ptr->SetSensor(true);
					p_sprites.back().setColor(0, 0, 0, 0);
				}
				break;
			}
		}
		std::string s;
		switch (text.getText()[0]) {
		default: break;
		case '{': bod->b2Ptr->SetLinearVelocity(b2Vec2(v.x / 4, v.y / 4)); break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			s = text.getText();
			--s[0];
			text.setText(s);
			text2.setText(s);
			text2[0].canvas.rotationAngle += fk::TAU / 2.0;
			text[0].setColor(255, 255, 0, 128);
			text2[0].setColor(255, 255, 0, 128);
			break;
		case '0':
			reactors.push_back(nullptr);
			break;
		case '*':
			if (limbs.size() > 1) {
				health = 0;
				bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 10, u.y * 10.0), true);
			}
			break;
		}
	// Player unrelated
	} else {
		auto block = static_cast<TextBlock*>(bod);
		switch (text.getText()[0]) {
		default: break;
		case '-':
		case 'x':
		case '+':
			if (block->text.getText()[0] == '~' || block->text.getText()[0] == 'c') { health = 0;  bod->health = 0; }
			break;
		case 'o':
			if (block->text.getText()[0] == 'o') reactors.push_back(block);
			break;
		case 'O':
			if (block->text.getText()[0] == 'O' ) reactors.push_back(block);
			break;
		case '*':
			if (limbs.size() > 1) {
				health = 0;
				bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 10.0, u.y * 10.0), true);
			}
			break;
		}
	}
}

void TextBlock::p_endCollision(b2Fixture * collisionFixturePtr, b2Fixture * myFixturePtr, b2Contact * contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	// Player related
	if (bod->type > 0) {
		// Player
		if (bod->type == 1) {
			switch (text.getText()[0]) {
			default: break;
			case 'w':
				limbs.back().b2Ptr->SetSensor(false);
				text[0].setColor(255, 255, 0, 128);
				text2[0].setColor(255, 255, 0, 128);
				p_sprites.back().setColor(255, 0, 0, 255);
				break;
			case '<':
				bod->gravMod.x += 10;
				p_sprites.back().setColor(0, 0, 0, 100);
				break;
			case '>':
				bod->gravMod.x -= 10;
				p_sprites.back().setColor(0, 0, 0, 100);
				break;
			case '^':
				bod->gravMod.y -= 10;
				p_sprites.back().setColor(0, 0, 0, 100);
				break;
			case 'v':
				bod->gravMod.y += 10;
				p_sprites.back().setColor(0, 0, 0, 100);
				break;
			}
		}
	} else {
		auto block = static_cast<TextBlock*>(bod);
		switch (text.getText()[0]) {
		default: break;
		case 'o':
			if (block->text.getText()[0] == 'o') reactors.remove(block);
			break;
		case 'O':
			if (block->text.getText()[0] == 'O') reactors.remove(block);
			break;
		}
	}
}