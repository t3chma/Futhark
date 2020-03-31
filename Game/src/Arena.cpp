#include "Arena.h"
#include <string>
#include "in/IOManager.h"

Arena::Arena(std::string& levelPath, fk::TTFont& f, fk::Texture& t, fk::SpriteBatch& sb, fk::World& world, Player::Def pd) {
	fk::IOManager iom;
	std::vector<std::string> buffer;
	iom.readTextFileToStringVector(levelPath, buffer);
	int lineIndex = 0;
	char mode = 'o';
	float nt = 0;
	float nx = 0;
	float ny = 0;
	float nr = 0;
	for (auto&& line: buffer) {
		map.emplace_back();
		int charIndex = 0;
		std::string number = "";
		fk::Random ranGen;
		int sign = 1;
		for (auto&& c : line) {
			switch (line[0]) {
			case 'W':
				switch (c) {
				default:
					mode = 'o';
					break;
				case 't':
					mode = c; nt = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case 'x':
					mode = c; nx = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case 'y':
					mode = c; ny = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case 'r':
					mode = c; nr = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case '-': sign = -sign; break;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '0': number += c; break; break;
				case ' ':
				case '\n':
					if (mode == 'r') {
						weather.push_back(Wind());
						weather.back().t = nt;
						weather.back().d = glm::vec2(ranGen.getFloat(-nr, nr), ranGen.getFloat(-nr, nr));
					} else {
						weather.push_back(Wind());
						weather.back().t = nt;
						weather.back().d = glm::vec2(nx, ny);
					}
					nt = 0; nx = 0; ny = 0; nr = 0; sign = 1;
				break;
				}
			break;
			case 'C':
				switch (c) {
				default: mode = 'o'; break;
				case 't':
					mode = c; nt = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case 'x':
					mode = c; nx = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case 'y':
					mode = c; ny = std::atoi(number.c_str()) * sign; number = ""; sign = 1; break;
				case '-':
				case '.':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case '0': number += c; break;
				case ' ':
				case '\n':
					choreography.push_back(Move());
					choreography.back().t = nt;
					choreography.back().step = glm::vec2(nx / nt, -ny / nt);
					break;
				}
			break;
			default:
				if (c > 32 && c < 127) {
					switch (c) {
					case '@': spawns.emplace_back((float)charIndex / 2.0, -(float)lineIndex / 2.0); break;
					default:
					case 'a': cam.y = -((float)lineIndex - 1) / 2.0; break;
					case 'b': cam.x = (float)charIndex / 2.0; break;
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
					case ':':
					case '=':
					case '|':
					case 'c':
					case 'w':
					case '{':
					case '}':
					case '[':
					case ']':
					case '#':
					case '/':
					case '*':
					case '~':
					case 'o':
					case 'O':
					case '<':
					case '>':
					case 'v':
					case '^':
					case 'p':
					case 'q':
					case '.':
						Body::Def bd;
						bd.position.y = -(float)lineIndex / 2.0;
						bd.position.x = (float)charIndex / 2.0;
						map.back().emplace_back(c, f, sb, world, bd, t);
					break;
					}
				}
				width = (++charIndex > width) ? charIndex : width;
			break;
			}
		}
		++lineIndex;
	}
	ctime = choreography[c].t;
	wtime = weather[w].t;
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
	if (choreography.size() && !freeze) {
		ctime -= 1;
		cam += choreography[c].step;
		if (ctime < 0) {
			++c;
			if (c >= choreography.size()) {
				c = 0; }
			ctime = choreography[c].t;
		};
	}
	if (weather.size()) {
		wtime -= 1;
		gravity = weather[w].d;
		if (wtime < 0) {
			++w;
			if (w >= weather.size()) {
				w = 0; }
			wtime = weather[w].t;
		};
	}
}