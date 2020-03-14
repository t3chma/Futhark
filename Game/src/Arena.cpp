#include "Arena.h"
#include <string>
#include "in/IOManager.h"

Arena::Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& w) {
	fk::IOManager iom;
	std::vector<std::string> buffer;
	iom.readTextFileToStringVector(levelPath, buffer);
	int lineIndex = 0;
	for (auto&& line: buffer) {
		map.emplace_back();
		int charIndex = 0;
		for (auto&& c : line) {
			if (line[charIndex] > 32 && line[charIndex] < 127) {
				switch (line[charIndex]) {
				default: line[charIndex] = 'z';
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
				case 's':
				case 'z':
				case 'x':
				case '+':
				case '-':
				case 'o':
				case ':':
				case 'O':
				case '~':
				case '=':
				case '|':
				case 'c':
					Body::Def bd;
					bd.position.y = -(float)lineIndex / 2.0;
					bd.position.x = (float)charIndex / 2.0;
					std::string s = "";
					s += line[charIndex];
					fk::TextSprite ts = f.generateCharSprites(s, sb, glm::vec2(0.5));
					ts.setDepth(1);
					map.back().emplace_back(ts, sb, w, bd, t);
					break;
				}
			}
			++charIndex;
		}
		++lineIndex;
	}
}

Arena::~Arena() {
}

void Arena::draw() {
	for (auto&& line : map) {
		for (auto&& block : line) {
			block.draw();
		}
	}
}

void Arena::update(fk::UserInput& ui) {
	int pops = 0;
	for (auto&& line : map) {
		for (auto&& block : line) {
			block.update(ui);
		}
		line.remove_if([](TextBlock& tb) { return tb.health < 1; });
	}
}