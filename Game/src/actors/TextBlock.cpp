#include "TextBlock.h"
#include "up/World.h"


TextBlock::TextBlock(fk::TextSprite& text, fk::SpriteBatch& sb, fk::World& w, Body::Def& bd, fk::Texture& t)
	: Image(sb), text(text), Body(w, bd) {
	health = 10;
	type = 0;
	text.setPosition(bd.position);
	p_sprites.emplace_back(sb, t);
	p_sprites.back().setDimensions(0.5, 0.5);
	p_sprites.back().setColor(0, 0, 0, 255);
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
	case '0': text[0].setColor(255, 255, 0, 128); b2Ptr->SetLinearDamping(10); break;
	case '-':
	case '+':
		b2Ptr->SetLinearDamping(10);
		text[0].setColor(255, 255, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 255);
		break;
	case '~':
		limbs.back().b2Ptr->SetDensity(1);
		text[0].setColor(0, 255, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 0);
		break;
	case 'x':
		b2Ptr->SetLinearDamping(0);
		limbs.back().b2Ptr->SetDensity(1);
		limbs.back().b2Ptr->SetRestitution(.7);
		text[0].setColor(255, 0, 0, 255);
		p_sprites.back().setColor(255, 0, 0, 0);
		break;
	case 'c':
	case 'O':
	case 'o':
		b2Ptr->SetLinearDamping(5);
		text[0].setColor(0, 0, 255, 255);
		p_sprites.back().setColor(128, 128, 128, 0);
		break;
	case 'z':
	case '=':
	case 's':
		b2Ptr->SetLinearDamping(10);
		p_sprites.back().setColor(0, 0, 0, 255);
		text[0].setColor(128, 128, 128, 128);
		break;
	case ':': text[0].setColor(128, 128, 128, 255); break;
	}
}

TextBlock::~TextBlock() {
	text.clearText();
}

void TextBlock::setChar(char c) {
	std::string s = "" + c;
	text.setText(s);
}

void TextBlock::draw() {
	auto position = b2Ptr->GetPosition();
	p_sprites.back().setPosition(position.x, position.y);
	text.setPosition(glm::vec2(position.x, position.y));
	text[0].setRotationAxis(position.x, position.y);
	text[0].canvas.rotationAngle += fk::TAU / 4;
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
	switch (text.getText()[0]) {
	default: break;
	case 'o':
		if (reactors.size() > 2) {
			for (auto&& r : reactors) { r->health = 0; }
			std::string s = "O";
			text.setText(s);
			text[0].setColor(0, 0, 255, 255);
			p_sprites.back().setColor(128, 128, 128, 0);
			limbs.pop_back();
			addCircleLimb(0.24);
			limbs.back().b2Ptr->SetDensity(20);
			reactors.clear();
		} else if (reactors.size()) { break; }
		i = 0; j = 0; k = 0;
		break;
	case 'O':
		if (reactors.size() > 1) {
			for (auto&& r : reactors) { r->health = 0; }
			std::string s = "c";
			text.setText(s);
			text[0].setColor(0, 0, 255, 255);
			p_sprites.back().setColor(128, 128, 128, 0);
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
	case '9':
	case '0': i = 200; j = 100; break;
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
		// Player
		if (bod->type == 1) {
			switch (text.getText()[0]) {
			default: break;
			case '-':
			case '+':
			case 'x': --bod->health; break;
			case 'c':
			case 'o':
				bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x, u.y), true);
				b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, -u.y), true);
			case 'O':
				if (abs(u.x) > abs(u.y)) {
					bod->b2Ptr->ApplyForceToCenter(b2Vec2(u.x * 5, 0), true);
					b2Ptr->ApplyForceToCenter(b2Vec2(-u.x, 0), true);
					break;
				} else {
					bod->b2Ptr->ApplyForceToCenter(b2Vec2(0, u.y * 5), true);
					b2Ptr->ApplyForceToCenter(b2Vec2(0, -u.y), true);
					break;
				}
			}
		// Player made
		} else {
			switch (text.getText()[0]) {
			default: break;
			case '-':
			case ':':
				--health;
				p_sprites.front().getCanvasRef().color.a -= 20;
				break;
			}
		}
		std::string s;
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
			s = text.getText();
			--s[0];
			text.setText(s);
			text[0].setColor(255, 255, 0, 128);
			break;
		case '0':
			s = "x";
			text.setText(s);
			b2Ptr->SetLinearDamping(0);
			limbs.back().b2Ptr->SetDensity(1);
			text[0].setColor(255, 0, 0, 255);
			p_sprites.back().setColor(255, 0, 0, 0);
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
			if (block->text.getText()[0] == '~') { health = 0;  bod->health = 0; }
			break;
		case 'o':
			if (block->text.getText()[0] == 'o') reactors.push_back(block);
			break;
		case 'O':
			if (block->text.getText()[0] == 'O' ) reactors.push_back(block);
			break;
		}
	}
}

void TextBlock::p_endCollision(b2Fixture * collisionFixturePtr, b2Fixture * myFixturePtr, b2Contact * contactPtr) {
	Body* bod = static_cast<Body*>(collisionFixturePtr->GetBody()->GetUserData());
	// Player related
	if (bod->type > 0) {
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